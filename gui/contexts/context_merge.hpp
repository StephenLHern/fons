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
    class context_merge : public wxPanel, private settings_observer, private git::git_observer
    {
      public:
        context_merge(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git);
        ~context_merge() override;

      private:
        wxDataViewListCtrl *pull_request_view;
        void on_repo_select(std::string_view found_repo) override;
        void on_pull_request_found(std::string_view title, std::string_view url, std::string_view user_login) override;
        app_settings *subscribed_settings;
        git::git_mediator *subscribed_git;
    };
} // namespace fons::gui