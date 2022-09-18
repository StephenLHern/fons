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
        pull_request_event(wxEventType eventType, int event_id, std::string input_title, std::string input_url,
                           std::string input_user_login)
            : fons::events::cmd_event(eventType, event_id)
        {
            title = input_title;
            url = input_url;
            user_login = input_user_login;
        }

        // You *must* copy here the data to be transported
        pull_request_event(const pull_request_event &event) : fons::events::cmd_event(event)
        {
            title = event.title;
            url = event.url;
            user_login = event.user_login;
        }

        // Required for sending with wxPostEvent()
        virtual wxEvent *Clone() const override
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
        virtual void execute() override;
        std::string remote_url;

      private:
        void device_authorization();
    };
} // namespace fons::git