#pragma once

#include <chrono>
#include <thread>

#include "common/command.hpp"

namespace fons
{
    class mock_command : public fons::common::command
    {
      public:
        explicit mock_command(unsigned int processing_duration) : m_processing_duration(processing_duration){};

        void execute() override
        {
            for (unsigned int remaining_time = m_processing_duration; remaining_time > 0; --remaining_time)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

                if (status == common::cmd_status::cancelled)
                    break;
            }
        };

        unsigned int m_processing_duration;
    };
} // namespace fons