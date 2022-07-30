#pragma once

#include <vector>

#include "app_settings.hpp"
#include "common/command.hpp"
#include "git/unique_libgit_ptr.hpp"

namespace fons::git
{
    struct file_diff
    {
        std::string old_file_path;
        std::string old_file_content;
        std::string new_file_path;
        std::string new_file_conent;

        file_diff(std::string old_file_path_input, std::string old_file_content_input, std::string new_file_path_input,
                  std::string new_file_conent_input)
            : old_file_path(old_file_path_input), old_file_content(old_file_content_input), new_file_path(new_file_path_input),
              new_file_conent(new_file_conent_input){};
    };

    class head_to_workdir : public fons::common::command
    {
      public:
        head_to_workdir(){};
        virtual void execute() override;
        std::vector<file_diff> modified_file_list;
    };
} // namespace fons::git