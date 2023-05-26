#pragma once

#include <wx/event.h>

#include "cmd_event.hpp"

namespace fons::events
{
    class git_found_remote_event;
    wxDECLARE_EVENT(EVENT_GIT_FOUND_REMOTE, git_found_remote_event);

    class git_found_remote_event : public cmd_event
    {
      public:
        git_found_remote_event(wxEventType eventType, int event_id, std::string_view input_name, std::string_view input_url)
            : cmd_event(eventType, event_id), remote_name(input_name), remote_url(input_url)
        {
        }

        git_found_remote_event(const git_found_remote_event &event) = default;

        wxEvent *Clone() const override
        {
            return new git_found_remote_event(*this);
        }

        std::string remote_name;
        std::string remote_url;
    };
} // namespace fons::events