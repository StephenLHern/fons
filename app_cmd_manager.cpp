#include <ranges>
#include <thread>

#include "app_cmd_manager.hpp"

namespace fons
{
    void app_cmd_manager::init(app_main *bind_app, app_settings *bind_settings)
    {
        app = bind_app;
        settings = bind_settings;
        control_thread = std::thread{&app_cmd_manager::cmd_processing_task, this};
    }

    void app_cmd_manager::shutdown()
    {
        // Signal exit to control thread
        shutdown_requested = true;
        process_shutdown();
        process_cmds();

        // Join control thread
        if (control_thread.joinable())
            control_thread.join();
    }

    uint64_t app_cmd_manager::execute(std::shared_ptr<common::command> cmd)
    {
        std::scoped_lock lock(cmd_queue_mutex);
        cmd->app = app;
        cmd->app_settings = settings;
        cmd->app_cmd_manager = this;
        cmd_queue.push_back(cmd_instance{cmd});
        process_cmds();
        return cmd->id();
    }

    void fons::app_cmd_manager::cancel(uint64_t cmd_id)
    {
        auto match_id = [&cmd_id](const cmd_instance &ci) { return ci.cmd && cmd_id == ci.cmd->id(); };

        // If the cancelled cmd is found in the queued commands, remove it from the queue and mark it cancelled
        {
            std::scoped_lock lock(cmd_queue_mutex);

            for (auto &to_cancel : cmd_queue | std::views::filter(match_id))
            {
                to_cancel.cmd->status = common::cmd_status::cancelled;
                cmd_queue.erase(std::remove(cmd_queue.begin(), cmd_queue.end(), to_cancel), cmd_queue.end());
            }
        }

        // If the cancelled cmd is currently being executed, mark it cancelled so the cmd logic can terminate
        {
            std::scoped_lock lock(active_cmds_mutex);

            for (auto &to_cancel : active_cmds | std::views::filter(match_id))
                to_cancel.cmd->status = common::cmd_status::cancelled;
        }
    }

    void app_cmd_manager::process_cmds()
    {
        cmd_processing_condition.notify_one();
    }

    void app_cmd_manager::cmd_processing_task()
    {
        while (1)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Shutdown handling
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // Once a shutdown has been requested, prevent new commands from being executed
            if (shutdown_requested && process_shutdown())
                return;

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Wait on command processing need
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // Determine if queue is empty and all active commands are currently executing
            bool processing_wait = false;
            {
                std::scoped_lock lock(active_cmds_mutex, cmd_queue_mutex);
                auto cmd_active = [](cmd_instance const &instance) { return instance.cmd->status == common::cmd_status::active; };
                processing_wait = cmd_queue.empty() && std::all_of(active_cmds.begin(), active_cmds.end(), cmd_active);
            }

            if (processing_wait && !shutdown_requested)
            {
                std::unique_lock<std::mutex> lock{control_task_mutex};
                cmd_processing_condition.wait(lock);
                continue;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Process commands
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // Remove top item from queue
            bool cmd_queued = false;
            cmd_instance queue_front{};
            {
                std::scoped_lock lock(cmd_queue_mutex);
                cmd_queued = !cmd_queue.empty();
                if (cmd_queued)
                {
                    queue_front = std::move(*cmd_queue.begin());
                    cmd_queue.erase(cmd_queue.begin());
                }
            }

            {
                std::scoped_lock lock(active_cmds_mutex);

                if (cmd_queued)
                {
                    queue_front.thread = std::thread{&app_cmd_manager::cmd_task, this, queue_front.cmd->id()};
                    queue_front.cmd->status = common::cmd_status::active;
                    active_cmds.push_back(std::move(queue_front));
                }

                for (auto &active_cmd : active_cmds)
                {
                    if (active_cmd.cmd->status == common::cmd_status::complete)
                    {
                        if (active_cmd.thread->joinable())
                        {
                            active_cmd.thread->join();
                            active_cmd.cmd->status = common::cmd_status::joined;
                        }

                        for (cmd_observer *current_observer : active_cmd.cmd->observers)
                            current_observer->on_command_complete(active_cmd.cmd->id());
                    }
                }

                auto cmd_joined = [](const cmd_instance &i) { return i.cmd->status == common::cmd_status::joined; };
                active_cmds.erase(std::remove_if(active_cmds.begin(), active_cmds.end(), cmd_joined), active_cmds.end());
            }
        }
    }

    bool app_cmd_manager::process_shutdown()
    {
        auto cmd_active = [](cmd_instance const &instance) { return instance.cmd->status == common::cmd_status::active; };
        std::scoped_lock lock(active_cmds_mutex, cmd_queue_mutex);

        // Remove any queued commands
        cmd_queue.clear();

        // Cancel any executing commands, and wait for them to complete
        for (auto &active_cmd : active_cmds | std::views::filter(cmd_active))
            active_cmd.cmd->status = common::cmd_status::cancelled;

        return active_cmds.empty();
    }

    void app_cmd_manager::cmd_task(uint64_t cmd_id)
    {
        auto match_id = [&cmd_id](const cmd_instance &ci) { return ci.cmd && cmd_id == ci.cmd->id(); };

        std::unique_lock active_cmds_lock(active_cmds_mutex);

        auto task_command = std::find_if(active_cmds.begin(), active_cmds.end(), match_id);

        if (task_command == active_cmds.end())
            return;

        // Unlock the active commands mutex while cmd is executing
        active_cmds_lock.unlock();
        task_command->cmd->do_execute();
    }
} // namespace fons