#include "status.hpp"
#include "app_main.hpp"

#include "git/gitlib_manager.hpp"
#include "git/unique_libgit_ptr.hpp"

namespace fons::git
{
    wxDEFINE_EVENT(EVENT_STATUS, wxCommandEvent);

    void status::execute()
    {
        gitlib_manager gitlib;
        unique_repo_ptr repo(git_repository_open, git_repository_free, app->settings.active_repo.c_str());

        if (!repo)
            return;

        unique_ref_ptr ref(git_repository_head, git_reference_free, repo.get());

        if (!ref)
            return;

        const char *branch_name{};
        int git_result = git_branch_name(&branch_name, ref.get());

        if (git_result != GIT_OK)
            return;

        wxCommandEvent *status_event = new wxCommandEvent(EVENT_STATUS);
        status_event->SetString(wxString(branch_name));
        wxQueueEvent(dynamic_cast<wxEvtHandler *>(app), status_event);
    }
} // namespace fons::git