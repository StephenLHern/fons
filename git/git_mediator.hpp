#pragma once

#include <memory>
#include <wx/event.h>

#include "common\cmd_observer.hpp"
#include "git_observer.hpp"
#include "settings_observer.hpp"

namespace fons
{
    class app_settings;
    class app_cmd_manager;

    namespace git
    {
        class find_repos;
        class find_branches;
        class revwalk;
        class revwalk_event;
    } // namespace git
} // namespace fons

namespace fons::git
{
    class git_mediator : public wxEvtHandler, settings_observer, cmd_observer
    {
      public:
        git_mediator(fons::app_settings *bind_settings, fons::app_cmd_manager *bind_cmd_manager);
        ~git_mediator();

        void subscribe(git_observer *observer);
        void unsubscribe(git_observer *observer);

        void init();
        void on_branch_found(wxCommandEvent &eventData);
        void on_status(wxCommandEvent &eventData);
        void on_commit_found(revwalk_event &eventData);
        virtual void on_repo_select(std::string selected_repo) override;
        virtual void on_command_complete(uint64_t command_id) override;

      private:
        fons::app_settings *settings;
        fons::app_cmd_manager *cmd_manager;

        std::vector<git_observer *> observers;

        std::weak_ptr<fons::git::find_repos> last_find_repos_cmd;
        std::weak_ptr<fons::git::find_branches> last_find_branches_cmd;
        std::weak_ptr<fons::git::revwalk> last_revwalk_cmd;

        std::string cached_active_branch;
    };
} // namespace fons::git