#pragma once

#include "common/cmd_observer.hpp"
#include "git/git_observer.hpp"

#include "gui/fons_wx.hpp"

namespace fons
{
    class app_settings;
    namespace git
    {
        class git_mediator;
        class head_to_workdir;
    } // namespace git
} // namespace fons

namespace fons::gui
{
    class context_changes : public wxPanel, private cmd_observer, private git::git_observer, private settings_observer
    {
      public:
        context_changes(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git, app_cmd_manager &cmd_manager);
        void on_command_complete(uint64_t command_id) override;
        void on_repo_select(std::string_view found_repo) override;

      private:
        std::shared_ptr<fons::git::head_to_workdir> last_head_to_workdir_cmd;
        app_cmd_manager *associated_cmd_manager;
        app_settings *subscribed_settings;
        git::git_mediator *subscribed_git;
    };
} // namespace fons::gui