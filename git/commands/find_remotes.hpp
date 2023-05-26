#pragma once

#include "common/command.hpp"

namespace fons::git
{
    class find_remotes : public fons::common::command
    {
      public:
        find_remotes(){};
        void execute() override;
    };
} // namespace fons::git