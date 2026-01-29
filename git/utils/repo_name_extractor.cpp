#include <algorithm>
#include <filesystem>
#include <git2.h>
#include <regex>
#include <string>

#include "repo_name_extractor.hpp"
#include "git/gitlib_manager.hpp"
#include "git/unique_libgit_ptr.hpp"

namespace fons::git::utils
{
    std::string extract_name_from_remote_url(const std::string& url)
    {
        // Handle various Git remote URL formats:
        // https://github.com/username/my-repo.git
        // git@github.com:username/my-repo.git
        // https://gitlab.com/group/project.git
        // ssh://git@gitlab.com/group/project.git

        try 
        {
            static const std::vector<std::regex> patterns = {
                // HTTPS URLs
                std::regex(R"(https?://[^/]+/([^/]+)/([^/]+?)(?:\.git)?$)"),
                // SSH URLs (git@host:user/repo.git)
                std::regex(R"(git@[^:]+:([^/]+)/([^/]+?)(?:\.git)?$)"),
                // SSH URLs (ssh://git@host/user/repo.git)
                std::regex(R"(ssh://git@[^/]+/([^/]+)/([^/]+?)(?:\.git)?$)")
            };

            for (const auto& pattern : patterns)
            {
                std::smatch matches;
                if (std::regex_match(url, matches, pattern) && matches.size() >= 3)
                {
                    return matches[1].str() + "/" + matches[2].str();
                }
            }
        }
        catch (const std::regex_error&)
        {
            // If regex fails, return empty string
        }

        return ""; // Could not extract name from URL
    }

    std::string extract_folder_name_from_path(const std::string& path)
    {
        // Extract the parent folder name from the repository path
        // Example: "C:\Users\Steph\source\my-repo\.git" -> "my-repo"
        
        std::filesystem::path git_path(path);
        std::filesystem::path repo_path = git_path.parent_path();
        
        if (repo_path.has_filename())
        {
            return repo_path.filename().string();
        }
        
        return path; // Fallback to the path itself
    }

    std::string get_origin_remote_url(const std::string& repo_path)
    {
        gitlib_manager gitlib;
        unique_repo_ptr repo(git_repository_open, git_repository_free, repo_path.c_str());

        if (!repo)
            return "";

        unique_config_ptr config(git_repository_config, git_config_free, repo.get());
        if (!config)
            return "";

        // Try to get the 'origin' remote URL
        const char* remote_url = nullptr;
        if (git_config_get_string(&remote_url, config.get(), "remote.origin.url") == GIT_OK && remote_url)
        {
            return std::string(remote_url);
        }

        return "";
    }

    std::string get_first_remote_url(const std::string& repo_path)
    {
        gitlib_manager gitlib;
        unique_repo_ptr repo(git_repository_open, git_repository_free, repo_path.c_str());

        if (!repo)
            return "";

        unique_str_array_ptr remotes(new git_strarray(), free_unique_str_array);

        if (git_remote_list(remotes.get(), repo.get()) != GIT_OK)
            return "";

        // Return the URL of the first remote found
        for (size_t i = 0; i < remotes.get()->count; ++i)
        {
            const char* remote_name = remotes.get()->strings[i];
            unique_remote_ptr remote(git_remote_lookup, git_remote_free, repo.get(), remote_name);

            if (remote.get())
            {
                const char* remote_url = git_remote_url(remote.get());
                if (remote_url)
                {
                    return std::string(remote_url);
                }
            }
        }

        return "";
    }

    std::string get_repo_display_name(const std::string& repo_path)
    {
        // Priority order:
        // 1. Extract name from 'origin' remote URL
        // 2. Extract name from first available remote URL
        // 3. Fall back to folder name

        // Try origin remote first
        std::string origin_url = get_origin_remote_url(repo_path);
        if (!origin_url.empty())
        {
            std::string name = extract_name_from_remote_url(origin_url);
            if (!name.empty())
                return name;
        }

        // Try first available remote
        std::string first_url = get_first_remote_url(repo_path);
        if (!first_url.empty())
        {
            std::string name = extract_name_from_remote_url(first_url);
            if (!name.empty())
                return name;
        }

        // Fall back to folder name
        return extract_folder_name_from_path(repo_path);
    }
}