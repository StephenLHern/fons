#pragma once
#include <optional>
#include <thread>

#include "common/command.hpp"

namespace fons
{
    class cmd_instance
    {
      public:
        cmd_instance() = default;

        explicit cmd_instance(std::shared_ptr<common::command> input_command) : cmd(input_command)
        {
        }

        bool operator==(cmd_instance const &rhs)
        {
            return cmd->id() == rhs.cmd->id();
        }

        std::shared_ptr<common::command> cmd;
        std::optional<std::jthread> thread = std::nullopt;
    };
} // namespace fons