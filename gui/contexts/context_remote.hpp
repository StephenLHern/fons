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
    class context_remote : public wxPanel, settings_observer, git::git_observer
    {
      public:
        context_remote(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git);
        virtual ~context_remote() override;

      private:
        wxDataViewListCtrl *repo_remote_view;
        virtual void on_repo_select(std::string found_repo) override;
        virtual void on_remote_found(std::string found_repo_name, std::string found_repo_url) override;
        app_settings *subscribed_settings;
        git::git_mediator *subscribed_git;
    };
} // namespace fons::gui