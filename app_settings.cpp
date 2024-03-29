#include "app_settings.hpp"

#include <filesystem>
#include <fstream>

#include <cereal/archives/json.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>

#include "common/optional_json.hpp"
#include "common/optional_nvp.hpp"
#include "git/gitlib_manager.hpp"
#include "git/unique_libgit_ptr.hpp"

namespace fons
{
    app_settings::app_settings()
    {
        load_settings();
        verify_settings();
    }

    app_settings::~app_settings()
    {
        save_settings();
    }

    void app_settings::subscribe(settings_observer *observer)
    {
        observers.push_back(observer);
    }

    void app_settings::unsubscribe(settings_observer *observer)
    {
        // Erase-remove idiom
        observers.erase(std::begin(std::ranges::remove(observers, observer)), observers.end());
    }

    void app_settings::on_repo_select(const wxCommandEvent &eventData)
    {
        // When event has been sent from a combo box, filter to the correct path format and only trigger an event if the user has selected a
        // different repo than was previously active
        if (eventData.GetEventType() == wxEVT_COMBOBOX)
        {
            auto stl_string = std::string(eventData.GetString());
            std::ranges::replace(stl_string, '/', '\\');
            stl_string += ".git";

            if (!repos.contains(stl_string) || stl_string == active_repo)
                return;

            active_repo = stl_string;
        }

        for (settings_observer *current_observer : observers)
            current_observer->on_repo_select(active_repo);
    }

    void app_settings::on_repo_found(const wxCommandEvent &eventData)
    {
       auto found_repo = std::string(eventData.GetString());

        if (repos.contains(found_repo))
            return;

        repos.insert(found_repo);

        for (settings_observer *current_observer : observers)
            current_observer->on_repo_found(found_repo);
    }

    template <class Archive>
    void app_settings::serialize(Archive &archive)
    {
        archive(CEREAL_OPTIONAL_NVP(active_repo), CEREAL_OPTIONAL_NVP(repos), CEREAL_OPTIONAL_NVP(last_repo_scan_time),
                CEREAL_OPTIONAL_NVP(github_access_token), CEREAL_OPTIONAL_NVP(github_token_type), CEREAL_OPTIONAL_NVP(github_token_scope));
    }

    void app_settings::load_settings()
    {
        std::ifstream settings_file;
        settings_file.open("settings.json");
        if (!settings_file.fail())
        {
            cereal::JSONInputArchive input(settings_file);
            input(cereal::make_nvp("app_settings", *this));
        }
    }

    void app_settings::save_settings() noexcept
    {
        std::ofstream settings_file;
        settings_file.open("settings.json");
        if (!settings_file.fail())
        {
            cereal::JSONOutputArchive output(settings_file);
            output(cereal::make_nvp("app_settings", *this));
        }
    }

    void fons::app_settings::verify_settings()
    {
        git::gitlib_manager gitlib;

        for (auto repo_it = repos.begin(); repo_it != repos.end();)
        {
            git::unique_repo_ptr repo(git_repository_open, git_repository_free, (*repo_it).c_str());
            repo_it = repo ? ++repo_it : repos.erase(repo_it);
        }
    }
} // namespace fons