#pragma once

#include <filesystem>
#include <vector>
#include <wx/event.h>

#include "app_settings.hpp"
#include "common\command.hpp"

namespace fons::git
{
    wxDECLARE_EVENT(EVENT_FOUND_REPO, wxCommandEvent);

    class find_repos : public fons::common::command
    {
      public:
        find_repos(){};
        virtual void execute() override;

      private:
        void find_repo_folder(const std::filesystem::path &dir_path);
    };
} // namespace fons::git