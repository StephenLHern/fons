#pragma once

#include "common/cmd_observer.hpp"
#include "git/git_observer.hpp"

#include "gui/fons_wx.h"

class wxTreeCtrl;

namespace fons
{
    class app_settings;
    namespace git
    {
        class git_mediator;
        class get_config;
    } // namespace git
} // namespace fons

namespace fons::gui
{
    class context_settings : public wxPanel, cmd_observer, git::git_observer, settings_observer
    {
      public:
        context_settings(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git, app_cmd_manager &cmd_manager);
        virtual void on_command_complete(uint64_t command_id) override;
        virtual void on_repo_select(std::string found_repo) override;

      private:
        std::shared_ptr<fons::git::get_config> last_get_config_cmd;
        app_cmd_manager *associated_cmd_manager;
        app_settings *subscribed_settings;
        git::git_mediator *subscribed_git;
        wxTreeCtrl *config_tree;
    };
} // namespace fons::gui