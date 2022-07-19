#include <gtest/gtest.h>

#include "app_cmd_manager.hpp"
#include "mocks/app_main.hpp"
#include "mocks/app_settings.hpp"
#include "mocks/mock_cmd.hpp"

namespace fons
{
    class cmd_manager_test_fixture : public ::testing::Test
    {
      protected:
        fons::app_cmd_manager cmd_manager;
        fons::app_settings settings;
        fons::app_main app;

        void SetUp() override
        {
            cmd_manager.init(&app, &settings);
        };

        void TearDown() override
        {
            cmd_manager.shutdown();
        }
    };

    TEST_F(cmd_manager_test_fixture, basic_cmd_execution)
    {
        auto test_cmd = std::make_shared<mock_command>(10);
        cmd_manager.execute(test_cmd);
        EXPECT_EQ(test_cmd->status, common::cmd_status::queued);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        EXPECT_EQ(test_cmd->status, common::cmd_status::joined);
    }

    TEST_F(cmd_manager_test_fixture, cancelled_cmd)
    {
        auto test_cmd = std::make_shared<mock_command>(10);
        cmd_manager.execute(test_cmd);
        EXPECT_EQ(test_cmd->status, common::cmd_status::queued);
        cmd_manager.cancel(test_cmd->id());
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        EXPECT_EQ(test_cmd->status, common::cmd_status::cancelled);
    }

    TEST_F(cmd_manager_test_fixture, cancelled_cmd_on_shutdown)
    {
        auto test_cmd = std::make_shared<mock_command>(100);
        cmd_manager.execute(test_cmd);
    }

    TEST_F(cmd_manager_test_fixture, cancel_empty_command_queue)
    {
        auto test_cmd = std::make_shared<mock_command>(10);
        EXPECT_EQ(test_cmd->status, common::cmd_status::queued);
        cmd_manager.cancel(test_cmd->id());
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        EXPECT_EQ(test_cmd->status, common::cmd_status::queued);
    }

} // namespace fons