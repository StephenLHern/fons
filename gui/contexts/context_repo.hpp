#pragma once

#include "git/git_observer.hpp"
#include "gui/fons_wx.h"
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
    class context_repo : public wxPanel, settings_observer, git::git_observer
    {
      public:
        context_repo(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git);
        virtual ~context_repo() override;

      private:
        virtual void on_repo_select(std::string found_repo) override;
        virtual void on_commit_found(fons::git::commit found_commit) override;
        wxDataViewListCtrl *repo_commit_view;
        app_settings *subscribed_settings;
        git::git_mediator *subscribed_git;
    };
} // namespace fons::gui