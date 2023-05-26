#pragma once

#include "gui/fons_wx.hpp"
#include "settings_observer.hpp"

namespace fons
{
    class app_settings;
}

namespace fons::gui
{
    class repo_selector : public wxComboBox, private settings_observer
    {
      public:
        repo_selector(wxWindow *parent, fons::app_settings &settings);
        repo_selector(repo_selector& other) = delete;
        repo_selector &operator=(const repo_selector &other) = delete;
        ~repo_selector() override;
        void on_repo_found(std::string_view found_repo) override;

      private:
        wxArrayString repo_cache;
        app_settings *subscribed_settings;
    };
} // namespace fons::gui