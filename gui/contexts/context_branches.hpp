#pragma once

#include "git/git_observer.hpp"
#include "gui/fons_wx.hpp"
#include "settings_observer.hpp"

class wxDataViewListCtrl;

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
    class context_branches : public wxPanel, private settings_observer, private git::git_observer
    {
      public:
        context_branches(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git);
        context_branches(context_branches &other) = delete;
        context_branches &operator=(const context_branches &other) = delete;
        ~context_branches() override;

      private:
        wxDataViewListCtrl *repo_branch_view;
        void on_repo_select(std::string_view found_repo) override;
        void on_branch_found(std::string_view found_commit) override;
        app_settings *subscribed_settings;
        git::git_mediator *subscribed_git;
    };
} // namespace fons::gui