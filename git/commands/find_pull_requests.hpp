#pragma once

#include <wx/event.h>

#include "app_settings.hpp"
#include "common/command.hpp"

namespace fons::git
{
    class pull_request_event;
    wxDECLARE_EVENT(EVENT_FOUND_PULL_REQUEST, pull_request_event);
    wxDECLARE_EVENT(EVENT_USER_CODE_GENERATE, wxCommandEvent);

    class pull_request_event : public fons::events::cmd_event
    {
      public:
        pull_request_event(wxEventType eventType, int event_id, std::string_view input_title, std::string_view input_url,
                           std::string_view input_user_login)
            : fons::events::cmd_event(eventType, event_id), title(input_title), url(input_url), user_login(input_user_login)
        {
        }

        // You *must* copy here the data to be transported
        pull_request_event(const pull_request_event &event) = default;

        // Required for sending with wxPostEvent()
        wxEvent *Clone() const override
        {
            return new pull_request_event(*this);
        }

        std::string title;
        std::string url;
        std::string user_login;
    };

    class find_pull_requests : public fons::common::command
    {
      public:
        find_pull_requests(){};
        void execute() override;
        std::string remote_url;

      private:
        void device_authorization();
    };
} // namespace fons::git