#pragma once

#include <atomic>
#include <optional>
#include <thread>

#include "app_cmd_manager.hpp"
#include "app_settings.hpp"
#include "git/git_mediator.hpp"
#include "gui/fons_wx.h"

namespace fons
{
    class app_main : public wxApp
    {
      public:
        virtual bool OnInit() override;
        virtual int OnExit() override;

        void on_close(wxCloseEvent &event_data);

        app_settings settings;
        std::optional<git::git_mediator> git_mediator;

      private:
        app_cmd_manager cmd_manager;
    };
} // namespace fons