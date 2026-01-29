#pragma once

#include <string>

namespace fons::git::utils
{
    std::string get_repo_display_name(const std::string& repo_path);
    std::string extract_name_from_remote_url(const std::string& url);
    std::string extract_folder_name_from_path(const std::string& path);
    std::string get_origin_remote_url(const std::string& repo_path);
    std::string get_first_remote_url(const std::string& repo_path);
}