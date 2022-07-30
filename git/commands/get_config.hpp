#pragma once

#include <vector>
#include <wx/event.h>

#include "app_settings.hpp"
#include "common/command.hpp"
#include "git/config_entry.hpp"

namespace fons::git
{
    class get_config : public fons::common::command
    {
      public:
        get_config(){};
        virtual void execute() override;
        std::vector<config_entry> config_entries;
    };
} // namespace fons::git