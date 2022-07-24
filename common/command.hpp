#pragma once

#include "cmd_observer.hpp"
#include "events/cmd_event.hpp"
#include <atomic>
#include <vector>
#include <wx/event.h>

namespace fons
{
    class app_main;
    class app_settings;
    class app_cmd_manager;
} // namespace fons

namespace fons::common
{
    enum class resource_type
    {
        file_system = 0x00000001,
        scm_tool = 0x00000002,
        git_rebase_file = 0x00000004,
    };

    enum class cmd_status : uint8_t
    {
        queued = 0x00,
        active = 0x01,
        cancelled = 0x02,
        complete = 0x04,
        joined = 0x08
    };

    class command
    {
      public:
        command() : status(cmd_status::queued), app(nullptr), app_settings(nullptr), app_cmd_manager(nullptr), instance_id(++static_id){};

        command(cmd_observer *observer) : command()
        {
            subscribe(observer);
        }

        virtual ~command(){};

        std::atomic<cmd_status> status;

        uint64_t id()
        {
            return instance_id;
        }

        void subscribe(cmd_observer *observer);
        void unsubscribe(cmd_observer *observer);

        template <class EventT, class... EventConstructorArgs>
        void queue_event(wxEventType event_id = events::EVENT_COMMAND, EventConstructorArgs &&...args)
        {
            EventT *event_to_queue = new EventT(event_id, std::forward<EventConstructorArgs>(args)...);
            event_to_queue->parent_cmd_id = id();
            wxQueueEvent(dynamic_cast<wxEvtHandler *>(app), event_to_queue);
        }

        void virtual execute(){};
        void do_execute();
        void cancel();

      protected:
        fons::app_main *app;
        fons::app_settings *app_settings;
        fons::app_cmd_manager *app_cmd_manager;
        std::vector<cmd_observer *> observers;
        inline static uint64_t static_id = uint64_t{};
        const uint64_t instance_id;

        friend class fons::app_cmd_manager;
    };
} // namespace fons::common