#pragma once

#include "common/cmd_instance.hpp"
#include "common/command.hpp"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>

namespace fons
{
    class app_main;
    class app_settings;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class app_cmd_manager
    /// @brief Centralized manager for internal fons application commands.
    ///
    /// The app_cmd_manager class is responsible for creating threads to execute internal fons application commands, provide access to
    /// queued, executing commands, notify other objects of command completion, and allow cancellation of commands.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class app_cmd_manager
    {
      public:
        void init(app_main *bind_app, app_settings *bind_settings);
        void shutdown();
        uint64_t execute(std::shared_ptr<common::command> command);
        void cancel(uint64_t command_id);
        void process_cmds();

      private:
        app_main *app;
        app_settings *settings;

        std::atomic<bool> shutdown_requested = false;

        std::thread control_thread;
        std::condition_variable cmd_processing_condition;
        std::mutex control_task_mutex;

        std::vector<cmd_instance> cmd_queue;
        std::mutex cmd_queue_mutex;

        std::vector<cmd_instance> active_cmds;
        std::mutex active_cmds_mutex;

        void cmd_processing_task();
        bool process_shutdown();
        void cmd_task(uint64_t commandID);
    };
} // namespace fons