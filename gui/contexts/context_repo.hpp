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
    class context_repo : public wxPanel, private settings_observer, private git::git_observer
    {
      public:
        context_repo(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git);
        ~context_repo() override;

      private:
        void on_repo_select(std::string_view found_repo) override;
        void on_commit_found(const fons::git::commit &found_commit) override;
        wxDataViewListCtrl *repo_commit_view;
        app_settings *subscribed_settings;
        git::git_mediator *subscribed_git;
    };
} // namespace fons::gui