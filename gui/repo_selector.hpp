#pragma once

#include "gui/fons_wx.hpp"
#include "settings_observer.hpp"

namespace fons
{
    class app_settings;
}

namespace fons::gui
{
    class repo_selector : public wxComboBox, settings_observer
    {
      public:
        repo_selector(wxWindow *parent, fons::app_settings &settings);
        virtual ~repo_selector() override;
        virtual void on_repo_found(std::string found_repo) override;

      private:
        wxArrayString repo_cache;
        app_settings *subscribed_settings;
    };
} // namespace fons::gui