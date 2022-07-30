#pragma once

#include <vector>

#include "cmd_event.hpp"
#include "common/cmd_observer.hpp"

namespace fons::events
{
    class cmd_complete_event;
    wxDECLARE_EVENT(EVENT_CMD_COMPLETE, cmd_complete_event);

    class cmd_complete_event : public cmd_event
    {
      public:
        cmd_complete_event(wxEventType eventType = EVENT_CMD_COMPLETE, int event_id = 0,
                           std::vector<cmd_observer *> input_observers = std::vector<cmd_observer *>())
            : cmd_event(eventType, event_id)
        {
            observers = input_observers;
        }

        cmd_complete_event(const cmd_complete_event &event) : cmd_event(event)
        {
            observers = std::vector<cmd_observer *>(event.observers);
        }

        wxEvent *Clone() const
        {
            return new cmd_complete_event(*this);
        }

        std::vector<cmd_observer *> observers;
    };
} // namespace fons::events