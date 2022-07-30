#pragma once

#include <git2.h>
#include <string>

namespace fons::git
{
    class config_entry
    {
      public:
        config_entry()
        {
        }

        config_entry(unsigned int input_include_depth, git_config_level_t input_level, std::string input_name, std::string input_value)
            : include_depth(input_include_depth), level(input_level), name(input_name), value(input_value)
        {
        }

        config_entry(const config_entry &copy)
        {
            include_depth = copy.include_depth;
            level = copy.level;
            name = copy.name;
            value = copy.value;
        }

        unsigned int include_depth;
        git_config_level_t level;
        std::string name;
        std::string value;
    };
} // namespace fons::git