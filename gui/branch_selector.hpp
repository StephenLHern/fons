#pragma once

#include "git/git_observer.hpp"
#include "gui/fons_wx.hpp"
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
    class branch_selector : public wxComboBox, private settings_observer, private git::git_observer
    {
      public:
        branch_selector(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git);
        branch_selector(branch_selector &other) = delete;
        branch_selector &operator=(const branch_selector &other) = delete;
        ~branch_selector() override;

        void on_repo_select(std::string_view found_repo) override;
        void on_branch_select(const wxCommandEvent &event_data);
        void on_branch_found(std::string_view found_branch) override;
        void on_status_change(std::string_view active_branch) override;

      private:
        wxArrayString branch_cache;
        app_settings *subscribed_settings;
        git::git_mediator *subscribed_git;
    };
} // namespace fons::gui