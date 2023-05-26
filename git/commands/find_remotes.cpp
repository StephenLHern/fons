#include "find_remotes.hpp"
#include "app_main.hpp"
#include "events/git_found_remote_event.hpp"
#include "git/gitlib_manager.hpp"
#include "git/unique_libgit_ptr.hpp"

namespace fons::events
{
    wxDEFINE_EVENT(EVENT_GIT_FOUND_REMOTE, git_found_remote_event);
}

namespace fons::git
{
    void find_remotes::execute()
    {
        gitlib_manager gitlib;
        unique_repo_ptr repo(git_repository_open, git_repository_free, app->settings.active_repo.c_str());

        if (!repo)
            return;

        unique_str_array_ptr remotes(new git_strarray(), free_unique_str_array);

        if (git_remote_list(remotes.get(), repo.get()) != GIT_OK && status != common::cmd_status::cancelled)
            return;

        for (size_t current_index = 0; current_index < remotes.get()->count && status != common::cmd_status::cancelled; ++current_index)
        {
            const char *current_remote = remotes.get()->strings[current_index];

            unique_remote_ptr remote(git_remote_lookup, git_remote_free, repo.get(), current_remote);

            if (!remote.get())
                continue;

            const char *current_url = git_remote_url(remote.get());
            queue_event<events::git_found_remote_event>(events::EVENT_GIT_FOUND_REMOTE, wxID_ANY, current_remote, current_url);
        }
    }
} // namespace fons::git