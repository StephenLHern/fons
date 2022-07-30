#pragma once

#include <vector>
#include <wx/event.h>

#include "app_settings.hpp"
#include "common/command.hpp"

namespace fons::git
{
    wxDECLARE_EVENT(EVENT_FOUND_BRANCH, wxCommandEvent);

    class find_branches : public fons::common::command
    {
      public:
        find_branches(){};
        virtual void execute() override;
    };
} // namespace fons::git