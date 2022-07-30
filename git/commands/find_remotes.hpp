#pragma once

#include "common/command.hpp"

namespace fons::git
{
    class find_remotes : public fons::common::command
    {
      public:
        find_remotes(){};
        virtual void execute() override;
    };
} // namespace fons::git