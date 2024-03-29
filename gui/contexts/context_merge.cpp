#include <wx/dataview.h>

#include "app_settings.hpp"
#include "context_merge.hpp"
#include "git/git_mediator.hpp"

namespace fons::gui
{
    context_merge::context_merge(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git) : wxPanel(parent)
    {
        bound_settings.subscribe(this);
        subscribed_settings = &bound_settings;

        bound_git.subscribe(this);
        subscribed_git = &bound_git;

        pull_request_view = new wxDataViewListCtrl(this, wxID_ANY);
        pull_request_view->AppendTextColumn("Title");
        pull_request_view->AppendTextColumn("URL");
        pull_request_view->AppendTextColumn("User");

        auto context_horizontal = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        context_horizontal->AddSpacer(5);
        context_horizontal->Add(pull_request_view, wxSizerFlags(1).Expand());

        auto context_vertical = new wxBoxSizer(wxOrientation::wxVERTICAL);
        context_vertical->AddSpacer(5);
        context_vertical->Add(context_horizontal, wxSizerFlags(1).Expand());

        SetSizerAndFit(context_vertical);
    }

    context_merge::~context_merge()
    {
        subscribed_settings->unsubscribe(this);
        subscribed_git->unsubscribe(this);
    }

    void context_merge::on_repo_select([[maybe_unused]] std::string_view found_repo)
    {
        pull_request_view->DeleteAllItems();
    }

    void context_merge::on_pull_request_found(std::string_view title, std::string_view url, std::string_view user_login)
    {
        wxVector<wxVariant> remote_data;
        remote_data.push_back(title.data());
        remote_data.push_back(url.data());
        remote_data.push_back(user_login.data());
        pull_request_view->AppendItem(remote_data);
    }
} // namespace fons::gui