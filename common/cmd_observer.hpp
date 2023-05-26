#pragma once

#include <cstdint>

namespace fons
{
    class cmd_observer
    {
      public:
        virtual ~cmd_observer() = default;

        virtual void on_command_complete([[maybe_unused]] uint64_t command_id) = 0;
    };
} // namespace fons