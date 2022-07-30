#pragma once

#include <string>

#include "commit.hpp"

namespace fons::git
{
    class git_observer
    {
      public:
        virtual ~git_observer() = default;

        virtual void on_remote_found(std::string found_remote_name, std::string found_remote_url){};
        virtual void on_branch_found(std::string found_branch){};
        virtual void on_status_change(std::string active_branch){};
        virtual void on_commit_found(commit found_commit){};
    };
} // namespace fons::git