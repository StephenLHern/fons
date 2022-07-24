#pragma once

#include <wx/event.h>

#include "app_settings.hpp"
#include "common/command.hpp"
#include "git/git_observer.hpp"

namespace fons::git
{
    class revwalk_event;
    wxDECLARE_EVENT(EVENT_REVWALK, revwalk_event);
    const int EVENT_FOUND_COMMIT = 0;

    class revwalk_event : public fons::events::cmd_event
    {
      public:
        revwalk_event(wxEventType eventType, int event_id, commit input_commit_data) : fons::events::cmd_event(eventType, event_id)
        {
            commit_data = input_commit_data;
        }

        // You *must* copy here the data to be transported
        revwalk_event(const revwalk_event &event) : fons::events::cmd_event(event)
        {
            commit_data = event.commit_data;
        }

        // Required for sending with wxPostEvent()
        virtual wxEvent *Clone() const override
        {
            return new revwalk_event(*this);
        }

        commit commit_data;
    };

    class revwalk : public fons::common::command
    {
      public:
        revwalk(){};
        virtual void execute() override;
    };
} // namespace fons::git