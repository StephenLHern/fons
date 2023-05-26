#pragma once

#include <string>

namespace fons
{
    class settings_observer
    {
      public:
        virtual ~settings_observer() = default;

        virtual void on_repo_select([[maybe_unused]] std::string_view selected_repo){
            // This method can optionally be implemented to respond to this event
        };
        virtual void on_repo_found([[maybe_unused]] std::string_view found_repo){
            // This method can optionally be implemented to respond to this event
        };
    };
} // namespace fons