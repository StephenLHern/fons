#include <chrono>
#include <format>
#include <iostream>

#include <wx/dataview.h>

#include "app_settings.hpp"
#include "context_repo.hpp"
#include "git/git_mediator.hpp"

namespace fons::gui
{
    context_repo::context_repo(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git) : wxPanel(parent)
    {
        bound_settings.subscribe(this);
        subscribed_settings = &bound_settings;

        bound_git.subscribe(this);
        subscribed_git = &bound_git;

        repo_commit_view = new wxDataViewListCtrl(this, wxID_ANY);
        repo_commit_view->AppendTextColumn("Commit");
        repo_commit_view->AppendTextColumn("Author");
        repo_commit_view->AppendTextColumn("Time");
        repo_commit_view->AppendTextColumn("Message");

        wxBoxSizer *context_horizontal = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        context_horizontal->AddSpacer(5);
        context_horizontal->Add(repo_commit_view, wxSizerFlags(1).Expand());

        wxBoxSizer *context_vertical = new wxBoxSizer(wxOrientation::wxVERTICAL);
        context_vertical->AddSpacer(5);
        context_vertical->Add(context_horizontal, wxSizerFlags(1).Expand());

        SetSizerAndFit(context_vertical);
    }

    context_repo::~context_repo()
    {
        subscribed_settings->unsubscribe(this);
        subscribed_git->unsubscribe(this);
    }

    void context_repo::on_repo_select(std::string found_repo)
    {
        repo_commit_view->DeleteAllItems();
    }

    void context_repo::on_commit_found(fons::git::commit found_commit)
    {
        wxVector<wxVariant> repo_commit_data;
        repo_commit_data.push_back(found_commit.id);
        repo_commit_data.push_back(wxVariant(std::format("{} <{}>", found_commit.author_name, found_commit.author_email)));
        repo_commit_data.push_back(
            wxVariant(std::format("{:%d %b %Y %I:%M %p}", std::chrono::system_clock::from_time_t(found_commit.time_stamp))));
        repo_commit_data.push_back(wxVariant(found_commit.message));
        repo_commit_view->AppendItem(repo_commit_data);
    }
} // namespace fons::gui