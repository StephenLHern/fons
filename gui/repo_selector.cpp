#include <codecvt>
#include <filesystem>
#include <string>
#include <wx/combobox.h>
#include <wx/event.h>

#include "app_settings.hpp"
#include "git/commands/find_repos.hpp"
#include "repo_selector.hpp"

namespace fons::gui
{
    repo_selector::repo_selector(wxWindow *parent, app_settings &settings) : wxComboBox(parent, wxID_ANY)
    {
        for (std::string repo : settings.repos)
        {
            std::ranges::replace(repo, '\\', '/');
            if (repo.length() > 4)
                repo.erase(repo.end() - 4, repo.end());
            repo_cache.Add(wxString(repo));
        }
        Append(repo_cache);
        AutoComplete(repo_cache);

        if (!settings.active_repo.empty() && settings.active_repo.length() > 4)
        {
            std::string to_select = settings.active_repo;
            std::ranges::replace(to_select, '\\', '/');
            to_select.erase(to_select.end() - 4, to_select.end());
            SetValue(wxString(to_select));
        }

        Bind(wxEVT_COMBOBOX, &app_settings::on_repo_select, &settings);
        settings.subscribe(this);
        subscribed_settings = &settings;
    }

    repo_selector::~repo_selector()
    {
        subscribed_settings->unsubscribe(this);
    }

    void repo_selector::on_repo_found(std::string_view found_repo)
    {
        auto wxfound_repo = wxString(found_repo.data());
        Append(wxfound_repo);
        repo_cache.Add(wxfound_repo);
    }
} // namespace fons::gui