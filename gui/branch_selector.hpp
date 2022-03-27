#pragma once

#include "git/git_observer.hpp"
#include "gui/fons_wx.h"
#include "settings_observer.hpp"

namespace fons
{
    class app_settings;
    namespace git
    {
        class git_mediator;
    }
} // namespace fons

namespace fons::gui
{
    class branch_selector : public wxComboBox, settings_observer, git::git_observer
    {
      public:
        branch_selector(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git);
        virtual ~branch_selector() override;

        virtual void on_repo_select(std::string found_repo) override;
        void on_branch_select(wxCommandEvent &event_data);
        virtual void on_branch_found(std::string found_branch) override;
        virtual void on_status_change(std::string active_branch) override;

      private:
        wxArrayString branch_cache;
        app_settings *subscribed_settings;
        git::git_mediator *subscribed_git;
    };
} // namespace fons::gui