#pragma once

#include <filesystem>
#include <vector>
#include <wx/event.h>

#include "app_settings.hpp"
#include "common/command.hpp"

namespace fons::git
{
    wxDECLARE_EVENT(EVENT_STATUS, wxCommandEvent);

    class status : public fons::common::command
    {
      public:
        status(){};
        void execute() override;
    };
} // namespace fons::git