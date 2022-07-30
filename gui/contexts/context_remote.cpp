#include <wx/dataview.h>

#include "app_settings.hpp"
#include "context_remote.hpp"
#include "git/git_mediator.hpp"

namespace fons::gui
{
    context_remote::context_remote(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git) : wxPanel(parent)
    {
        bound_settings.subscribe(this);
        subscribed_settings = &bound_settings;

        bound_git.subscribe(this);
        subscribed_git = &bound_git;

        repo_remote_view = new wxDataViewListCtrl(this, wxID_ANY);
        repo_remote_view->AppendTextColumn("Remote");
        repo_remote_view->AppendTextColumn("Address");

        wxBoxSizer *context_horizontal = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        context_horizontal->AddSpacer(5);
        context_horizontal->Add(repo_remote_view, wxSizerFlags(1).Expand());

        wxBoxSizer *context_vertical = new wxBoxSizer(wxOrientation::wxVERTICAL);
        context_vertical->AddSpacer(5);
        context_vertical->Add(context_horizontal, wxSizerFlags(1).Expand());

        SetSizerAndFit(context_vertical);
    }

    context_remote::~context_remote()
    {
        subscribed_settings->unsubscribe(this);
        subscribed_git->unsubscribe(this);
    }

    void context_remote::on_repo_select(std::string found_repo)
    {
        repo_remote_view->DeleteAllItems();
    }

    void context_remote::on_remote_found(std::string found_repo_name, std::string found_repo_url)
    {
        wxVector<wxVariant> remote_data;
        remote_data.push_back(found_repo_name);
        remote_data.push_back(found_repo_url);
        repo_remote_view->AppendItem(remote_data);
    }
} // namespace fons::gui