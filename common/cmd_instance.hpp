#pragma once
#include <optional>
#include <thread>

#include "common/command.hpp"

namespace fons
{
    class cmd_instance
    {
      public:
        cmd_instance()
        {
            thread = std::nullopt;
        }

        cmd_instance(std::shared_ptr<common::command> input_command) : cmd_instance()
        {
            cmd = input_command;
        }

        bool operator==(cmd_instance const &rhs)
        {
            return cmd->id() == rhs.cmd->id();
        }

        std::shared_ptr<common::command> cmd;
        std::optional<std::thread> thread;
    };
} // namespace fons