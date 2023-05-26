#pragma once

#include <wx/event.h>

#include "gui/fons_wx.hpp"

namespace fons::events
{
    class cmd_event;
    wxDECLARE_EVENT(EVENT_COMMAND, cmd_event);

    class cmd_event : public wxCommandEvent
    {
      public:
        cmd_event(wxEventType commandType = EVENT_COMMAND, int id = 0, uint64_t input_parent_cmd_id = 0)
            : wxCommandEvent(commandType, id), parent_cmd_id(input_parent_cmd_id)
        {
        }

        // You *must* copy here the data to be transported
        cmd_event(const cmd_event &event) : wxCommandEvent(event), parent_cmd_id(event.parent_cmd_id)
        {
        }

        // Required for sending with wxPostEvent()
        wxEvent *Clone() const override
        {
            return new cmd_event(*this);
        }

        uint64_t parent_cmd_id;
    };
} // namespace fons::events