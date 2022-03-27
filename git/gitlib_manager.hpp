#pragma once

#include <git2.h>

namespace fons::git
{
    class gitlib_manager
    {
      public:
        gitlib_manager()
        {
            git_libgit2_init();
        }

        ~gitlib_manager()
        {
            git_libgit2_shutdown();
        }
    };
} // namespace fons::git