#pragma once

#include <atomic>
#include <optional>
#include <thread>

#include "app_cmd_manager.hpp"
#include "app_settings.hpp"
#include "git/git_mediator.hpp"
#include "gui/fons_wx.hpp"

namespace fons
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class app_main
    /// @brief Represents the fons application, manages application-wide settings, command, scm access.
    ///
    /// The app_main class represents the fons application. It owns the classes used to control application settings, scm access, and the
    /// command manager. These objects are initialized and cleaned according to the lifecycle of the app_main object
    ///
    /// The app_main class is derived from the wxWidgets [wxApp class](https://docs.wxwidgets.org/3.0/classwx_app.html).
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class app_main : public wxApp
    {
      public:
        bool OnInit() override;
        int OnExit() override;

        void on_close(wxCloseEvent &event_data);

        app_settings settings;
        std::optional<git::git_mediator> git_mediator;
        app_cmd_manager cmd_manager;
    };
} // namespace fons