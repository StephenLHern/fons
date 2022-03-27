#include <codecvt>
#include <filesystem>
#include <string>
#include <wx/combobox.h>
#include <wx/event.h>

#include "app_settings.hpp"
#include "branch_selector.hpp"
#include "git/commands/find_branches.hpp"
#include "git/git_mediator.hpp"

namespace fons::gui
{
    branch_selector::branch_selector(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git)
        : wxComboBox(parent, wxID_ANY)
    {
        Append(branch_cache);
        AutoComplete(branch_cache);

        Bind(wxEVT_COMBOBOX, &branch_selector::on_branch_select, this);

        bound_settings.subscribe(this);
        subscribed_settings = &bound_settings;

        bound_git.subscribe(this);
        subscribed_git = &bound_git;
    }

    branch_selector::~branch_selector()
    {
        subscribed_settings->unsubscribe(this);
        subscribed_git->unsubscribe(this);
    }

    void branch_selector::on_repo_select(std::string selected_repo)
    {
        Clear();
        branch_cache.clear();
        Enable(false);
    }

    void branch_selector::on_branch_select([[maybe_unused]] wxCommandEvent &eventData)
    {
    }

    void branch_selector::on_branch_found(std::string foundBranch)
    {
        wxString found_branch = wxString(foundBranch);
        branch_cache.Add(found_branch);
        Append(wxString(foundBranch));

        if (branch_cache.Count() > 1)
            Enable(true);
    }
    void branch_selector::on_status_change(std::string active_branch)
    {
        SetValue(wxString(active_branch));
    }
} // namespace fons::gui