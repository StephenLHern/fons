#include "find_repos.hpp"
#include "app_main.hpp"
#include "git/gitlib_manager.hpp"
#include "git/unique_libgit_ptr.hpp"

namespace fons::git
{
    wxDEFINE_EVENT(EVENT_FOUND_REPO, wxCommandEvent);

    void find_repos::execute()
    {
        std::filesystem::path root = std::filesystem::current_path().root_path();
        find_repo_folder(root);
    }

    void find_repos::find_repo_folder(const std::filesystem::path &dir_path)
    {
        if (!std::filesystem::exists(dir_path))
            return;

        std::filesystem::directory_iterator end_itr;
        std::error_code file_error;

        for (std::filesystem::recursive_directory_iterator dir(dir_path, std::filesystem::directory_options::skip_permission_denied), end;
             dir != end; dir.increment(file_error))
        {
            if (status == common::cmd_status::cancelled)
                return;

            if (file_error || dir->path().wstring().find(L"\u202E") != std::string::npos)
                continue;

            if (dir->path().filename() == ".git")
            {
                gitlib_manager gitlib;
                
                if (unique_repo_ptr repo(git_repository_open, git_repository_free, dir->path().string().c_str()); !repo)
                    continue;

                auto found_event = new wxCommandEvent(EVENT_FOUND_REPO);
                found_event->SetString(dir->path().string());
                wxQueueEvent(dynamic_cast<wxEvtHandler *>(app), found_event);
            }
        }
        return;
    }
} // namespace fons::git