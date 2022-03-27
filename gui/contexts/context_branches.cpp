#include "context_branches.hpp"
#include "app_settings.hpp"
#include "git/git_mediator.hpp"

#include <wx/dataview.h>

namespace fons::gui
{
    context_branches::context_branches(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git) : wxPanel(parent)
    {
        bound_settings.subscribe(this);
        subscribed_settings = &bound_settings;

        bound_git.subscribe(this);
        subscribed_git = &bound_git;

        repo_branch_view = new wxDataViewListCtrl(this, wxID_ANY);
        repo_branch_view->AppendTextColumn("Branch");

        wxBoxSizer *context_horizontal = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        context_horizontal->AddSpacer(5);
        context_horizontal->Add(repo_branch_view, wxSizerFlags(1).Expand());

        wxBoxSizer *context_vertical = new wxBoxSizer(wxOrientation::wxVERTICAL);
        context_vertical->AddSpacer(5);
        context_vertical->Add(context_horizontal, wxSizerFlags(1).Expand());

        SetSizerAndFit(context_vertical);
    }

    context_branches::~context_branches()
    {
        subscribed_settings->unsubscribe(this);
        subscribed_git->unsubscribe(this);
    }

    void context_branches::on_repo_select(std::string found_repo)
    {
        repo_branch_view->DeleteAllItems();
    }

    void context_branches::on_branch_found(std::string found_branch)
    {
        wxVector<wxVariant> repo_branch_data;
        repo_branch_data.push_back(wxVariant(found_branch));
        repo_branch_view->AppendItem(repo_branch_data);
    }
} // namespace fons::gui