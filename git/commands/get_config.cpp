#include <git2.h>

#include "app_main.hpp"
#include "get_config.hpp"
#include "git/gitlib_manager.hpp"
#include "git/unique_libgit_ptr.hpp"

namespace fons::git
{
    void get_config::execute()
    {
        gitlib_manager gitlib;

        unique_repo_ptr repo(git_repository_open, git_repository_free, app->settings.active_repo.c_str());

        if (!repo)
            return;

        unique_config_ptr config(git_repository_config, git_config_free, repo.get());

        if (!config)
            return;

        unique_config_itr config_iterator(git_config_iterator_new, git_config_iterator_free, config.get());
        git_config_entry *entry{};
        while (status != common::cmd_status::cancelled)
        {
            int iterate_result = git_config_next(&entry, config_iterator.get());

            if (iterate_result == GIT_ITEROVER)
                break;

            config_entries.push_back(config_entry(entry->include_depth, entry->level, entry->name, entry->value));

            if (entry->free)
                entry->free(entry);
        }
    }
} // namespace fons::git