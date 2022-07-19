#include <vector>
#include <wx/event.h>

#include "cmd_observer.hpp"

namespace fons
{
    class cmd_complete_event;
    wxDECLARE_EVENT(EVENT_CMD_COMPLETE, cmd_complete_event);

    class cmd_complete_event : public wxCommandEvent
    {
      public:
        cmd_complete_event(wxEventType commandType = EVENT_CMD_COMPLETE, int id = 0) : wxCommandEvent(commandType, id)
        {
            complete_cmd_id = 0;
        }

        cmd_complete_event(const cmd_complete_event &event) : wxCommandEvent(event)
        {
            observers = std::vector<cmd_observer *>(event.observers);
            complete_cmd_id = event.complete_cmd_id;
        }

        wxEvent *Clone() const
        {
            return new cmd_complete_event(*this);
        }

        std::vector<cmd_observer *> observers;
        uint64_t complete_cmd_id;
    };
} // namespace fons