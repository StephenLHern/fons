#pragma once

#include <string>

namespace fons::git
{
    class commit;

    class git_observer
    {
      public:
        virtual ~git_observer() = default;

        virtual void on_remote_found([[maybe_unused]] std::string_view found_remote_name,
                                     [[maybe_unused]] std::string_view found_remote_url){
            // This method can optionally be implemented to respond to this event
        };
        virtual void on_branch_found([[maybe_unused]] std::string_view found_branch){
            // This method can optionally be implemented to respond to this event
        };
        virtual void on_status_change([[maybe_unused]] std::string_view active_branch){
            // This method can optionally be implemented to respond to this event
        };
        virtual void on_commit_found([[maybe_unused]] const commit &found_commit){
            // This method can optionally be implemented to respond to this event
        };
        virtual void on_pull_request_found([[maybe_unused]] std::string_view title, [[maybe_unused]] std::string_view url,
                                           [[maybe_unused]] std::string_view user_login){
            // This method can optionally be implemented to respond to this event
        };
    };
} // namespace fons::git