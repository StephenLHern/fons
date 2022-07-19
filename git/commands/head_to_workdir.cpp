#include "head_to_workdir.hpp"
#include "app_main.hpp"
#include "git/gitlib_manager.hpp"
#include "git2/diff.h"

namespace fons::git
{
    void head_to_workdir::execute()
    {
        gitlib_manager gitlib;

        unique_repo_ptr repo(git_repository_open, git_repository_free, app->settings.active_repo.c_str());

        if (!repo || status == common::cmd_status::cancelled)
            return;

        unique_git_obj_ptr obj(git_revparse_single, git_object_free, repo.get(), "HEAD^{tree}");

        if (!obj || status == common::cmd_status::cancelled)
            return;

        unique_tree_ptr tree(git_tree_lookup, git_tree_free, repo.get(), git_object_id(obj.get()));

        if (!tree || status == common::cmd_status::cancelled)
            return;

        git_diff_options options;
        git_diff_init_options(&options, GIT_DIFF_OPTIONS_VERSION);

        unique_diff_ptr diff_data(git_diff_tree_to_workdir_with_index, git_diff_free, repo.get(), tree.get(), &options);

        if (!diff_data || status == common::cmd_status::cancelled)
            return;

        size_t diff_index = 0;
        while (status != common::cmd_status::cancelled)
        {
            const git_diff_delta *delta = git_diff_get_delta(diff_data.get(), diff_index);

            if (!delta)
                break;

            git_oid id_new;
            git_blob_create_from_workdir(&id_new, repo.get(), delta->new_file.path);

            unique_blob_ptr blob_new(git_blob_lookup, git_blob_free, repo.get(), &id_new);
            unique_blob_ptr blob_old(git_blob_lookup, git_blob_free, repo.get(), &delta->old_file.id);

            unique_buf_ptr filtered_old(new git_buf(GIT_BUF_INIT), free_unique_buf);
            git_blob_filter(filtered_old.get(), blob_old.get(), delta->old_file.path, nullptr);

            unique_buf_ptr filtered_new(new git_buf(GIT_BUF_INIT), free_unique_buf);
            git_blob_filter(filtered_new.get(), blob_new.get(), delta->new_file.path, nullptr);

            modified_file_list.push_back(file_diff(delta->old_file.path ? std::string(delta->old_file.path) : std::string(),
                                                   filtered_old->ptr ? std::string(filtered_old->ptr) : std::string(),
                                                   delta->new_file.path ? std::string(delta->new_file.path) : std::string(),
                                                   filtered_new->ptr ? std::string(filtered_new->ptr) : std::string()));
            diff_index++;
        }
    }
} // namespace fons::git