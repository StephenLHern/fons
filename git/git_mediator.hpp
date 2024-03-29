#pragma once

#include <memory>
#include <wx/event.h>

#include "common/cmd_observer.hpp"
#include "git_observer.hpp"
#include "settings_observer.hpp"

namespace fons
{
    class app_settings;
    class app_cmd_manager;

    namespace events
    {
        class git_found_remote_event;
    }

    namespace git
    {
        class find_remotes;
        class find_repos;
        class find_branches;
        class get_config;
        class revwalk;
        class revwalk_event;
        class pull_request_event;
    } // namespace git
} // namespace fons

namespace fons::git
{
    wxDECLARE_EVENT(EVENT_INIT_REPO, wxCommandEvent);

    class git_mediator : public wxEvtHandler, private settings_observer, private cmd_observer
    {
      public:
        git_mediator(fons::app_settings *bind_settings, fons::app_cmd_manager *bind_cmd_manager);
        ~git_mediator() override;

        void subscribe(git_observer *observer);
        void unsubscribe(git_observer *observer);

        void init();
        void on_branch_found(const wxCommandEvent &eventData);
        void on_status(const wxCommandEvent &eventData);
        void on_commit_found(const revwalk_event &eventData);
        void on_remote_found(const events::git_found_remote_event &eventData);
        void on_pull_request_found(const pull_request_event &eventData);
        void on_repo_select(std::string_view selected_repo) override;
        void on_command_complete(uint64_t command_id) override;

      private:
        fons::app_settings *settings;
        fons::app_cmd_manager *cmd_manager;

        std::vector<git_observer *> observers;

        std::weak_ptr<fons::git::find_repos> last_find_repos_cmd;
        std::weak_ptr<fons::git::find_branches> last_find_branches_cmd;
        std::weak_ptr<fons::git::revwalk> last_revwalk_cmd;
        std::weak_ptr<fons::git::find_remotes> last_find_remotes_cmd;
        std::weak_ptr<fons::git::get_config> last_get_config_cmd;

        std::string cached_active_branch;
    };
} // namespace fons::git