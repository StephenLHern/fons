#pragma once

#include <chrono>
#include <set>
#include <string>

#include <wx/event.h>

#include <cereal/access.hpp>

#include "settings_observer.hpp"

namespace fons
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class app_settings
    /// @brief Class for reading / writing fons application settings.
    ///
    /// The app_settings class is responsible for accessing and storing fons application settings. On construction it reads in currently
    /// stored application settings from the application settings file, on destruction it writes any changed settings to the application
    /// settings file.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class app_settings : public wxEvtHandler
    {
      public:
        app_settings();
        ~app_settings();

        void subscribe(settings_observer *observer);
        void unsubscribe(settings_observer *observer);

        void on_repo_select(wxCommandEvent &eventData);
        void on_repo_found(wxCommandEvent &eventData);

        // Repo Settings
        std::string active_repo;
        std::set<std::string> repos;
        std::chrono::time_point<std::chrono::system_clock> last_repo_scan_time;

        // GitHub API Settings
        std::string github_access_token;
        std::string github_token_type;
        std::string github_token_scope;

        std::vector<settings_observer *> observers;

        friend class cereal::access;

        template <class Archive>
        void serialize(Archive &archive);

      private:
        void load_settings();
        void save_settings();
        void verify_settings();
    };
} // namespace fons