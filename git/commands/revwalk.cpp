#include "revwalk.hpp"
#include "app_main.hpp"

#include "git/git_observer.hpp"
#include "git/gitlib_manager.hpp"
#include "git/unique_libgit_ptr.hpp"

namespace fons::git
{
    wxDEFINE_EVENT(EVENT_REVWALK, revwalk_event);

    void revwalk::execute()
    {
        gitlib_manager gitlib;
        unique_repo_ptr repo(git_repository_open, git_repository_free, app->settings.active_repo.c_str());

        unique_revwalk_ptr walk(git_revwalk_new, git_revwalk_free, repo.get());

        if (!walk.get())
            return;

        git_revwalk_sorting(walk.get(), GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);
        git_revwalk_push_head(walk.get());
        git_revwalk_hide_glob(walk.get(), "tags/*");

        unique_git_obj_ptr obj(git_revparse_single, git_object_free, repo.get(), "HEAD~1000");
        git_revwalk_hide(walk.get(), git_object_id(obj.get()));

        git_oid oid;
        while (git_revwalk_next(&oid, walk.get()) == 0 && status != common::cmd_status::cancelled)
        {
            unique_commit_ptr commit(git_commit_lookup, git_commit_free, repo.get(), &oid);

            char oidstr[GIT_OID_HEXSZ + 1]{};
            std::string id(git_oid_tostr(oidstr, sizeof(oidstr), &oid));
            std::string message((git_commit_message(commit.get())));
            std::string author_email(git_commit_author(commit.get())->email);
            std::string author_name(git_commit_author(commit.get())->name);
            __time64_t timestamp = git_commit_time(commit.get());

            fons::git::commit commit_data(id, message, author_name, author_email, timestamp);
            queue_event<revwalk_event>(EVENT_REVWALK, EVENT_FOUND_COMMIT, commit_data);
        }

        return;
    }
} // namespace fons::git