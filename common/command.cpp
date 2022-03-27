#include "command.hpp"
#include "app_main.hpp"

namespace fons::common
{
    void command::subscribe(cmd_observer *observer)
    {
        observers.push_back(observer);
    }

    void command::unsubscribe(cmd_observer *observer)
    {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void command::cancel()
    {
        if (status == common::cmd_status::active || status == common::cmd_status::queued)
            status = common::cmd_status::cancelled;
    }

    void command::do_execute()
    {
        execute();
        status = common::cmd_status::complete;
        app_cmd_manager->process_cmds();
    }
} // namespace fons::common