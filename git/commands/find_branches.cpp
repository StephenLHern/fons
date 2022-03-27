#include "find_branches.hpp"
#include "app_main.hpp"
#include "git/gitlib_manager.hpp"
#include "git/unique_libgit_ptr.hpp"

namespace fons::git
{
    wxDEFINE_EVENT(EVENT_FOUND_BRANCH, wxCommandEvent);

    void find_branches::execute()
    {
        gitlib_manager gitlib;
        unique_repo_ptr repo(git_repository_open, git_repository_free, app->settings.active_repo.c_str());

        if (!repo)
            return;

        unique_branch_itr branch_iterator(git_branch_iterator_new, git_branch_iterator_free, repo.get(), GIT_BRANCH_LOCAL);
        git_branch_t type;
        while (status != common::cmd_status::cancelled)
        {
            unique_ref_ptr ref(git_branch_next, git_reference_free, &type, branch_iterator.get());

            if (!ref)
                break;

            const char *branch_name{};
            int git_result = git_branch_name(&branch_name, ref.get());

            if (git_result != GIT_OK)
                continue;

            wxCommandEvent *found_event = new wxCommandEvent(EVENT_FOUND_BRANCH);
            found_event->SetString(wxString(branch_name));
            wxQueueEvent(dynamic_cast<wxEvtHandler *>(app), found_event);
        }
    }
} // namespace fons::git