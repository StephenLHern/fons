#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <regex>

#include "app_main.hpp"
#include "find_pull_requests.hpp"

using njson = nlohmann::json;

namespace fons::git
{
    wxDEFINE_EVENT(EVENT_FOUND_PULL_REQUEST, pull_request_event);
    wxDEFINE_EVENT(EVENT_USER_CODE_GENERATE, wxCommandEvent);

    template <class... GetArgs>
    std::optional<nlohmann::json> http_get(GetArgs &&...args)
    {
        cpr::Response get_response = cpr::Get(std::forward<GetArgs>(args)...);

        if (get_response.status_code != 200 || !get_response.header["content-type"].contains("application/json"))
            return std::nullopt;

        return njson::parse(get_response.text);
    }

    template <class... GetArgs>
    std::optional<nlohmann::json> http_post(GetArgs &&...args)
    {
        cpr::Response get_response = cpr::Post(std::forward<GetArgs>(args)...);

        if (get_response.status_code != 200 || !get_response.header["content-type"].contains("application/json"))
            return std::nullopt;

        return njson::parse(get_response.text);
    }

    void find_pull_requests::device_authorization()
    {
        auto verification_code_url = cpr::Url{"https://github.com/login/device/code"};
        auto verification_code_headers = cpr::Header{{"User-Agent", "Fons-Git-Client"}, {"Accept", "application/json"}};
        auto verification_code_parameters = cpr::Parameters{{"client_id", "70c00b0f7d91cc68a190"}, {"scope", "repo user"}};
        auto verification_codes_request_json = http_post(verification_code_url, verification_code_headers, verification_code_parameters);

        if (!verification_codes_request_json.has_value())
            return;

        std::string device_code = (*verification_codes_request_json)["device_code"];
        std::string user_code = (*verification_codes_request_json)["user_code"];
        std::string verification_uri = (*verification_codes_request_json)["verification_uri"];
        int expires_in = (*verification_codes_request_json)["expires_in"];
        int interval = (*verification_codes_request_json)["interval"];

        auto found_event = new wxCommandEvent(EVENT_USER_CODE_GENERATE);
        found_event->SetString(wxString(user_code));
        wxQueueEvent(dynamic_cast<wxEvtHandler *>(app), found_event);

        std::string access_token;
        std::string token_type;
        std::string scope;

        for (int timer = 0; timer <= expires_in; timer += interval)
        {
            auto access_token_url = cpr::Url{"https://github.com/login/oauth/access_token"};
            auto access_token_headers = cpr::Header{{"User-Agent", "Fons-Git-Client"}, {"Accept", "application/json"}};
            auto access_token_parameters = cpr::Parameters{{"client_id", "70c00b0f7d91cc68a190"},
                                                           {"device_code", device_code},
                                                           {"grant_type", "urn:ietf:params:oauth:grant-type:device_code"}};
            auto poll_access_token_request_json = http_post(access_token_url, access_token_headers, access_token_parameters);

            if (poll_access_token_request_json.has_value() && !((*poll_access_token_request_json).contains("error") &&
                                                                (*poll_access_token_request_json)["error"] == "authorization_pending"))
            {

                access_token = (*poll_access_token_request_json)["access_token"];
                token_type = (*poll_access_token_request_json)["token_type"];
                scope = (*poll_access_token_request_json)["scope"];
                break;
            }

            std::this_thread::sleep_for(std::chrono::seconds(interval));
        }

        app->settings.github_access_token = access_token;
        app->settings.github_token_type = token_type;
        app->settings.github_token_scope = scope;
    }

    void find_pull_requests::execute()
    {
        // A remote url must be provided
        if (remote_url.empty())
            return;

        // For now, only github remotes are supported with this command
        std::regex github_remote(".+github\\.com\\/([^\\/]+\\/[^\\/]+)\\.git", std::regex_constants::icase);
        std::smatch results;

        if (!std::regex_match(remote_url, results, github_remote) || !results[1].matched)
            return;

        // Todo: authorization should be managed with a proper state machine and user account manager
        if (app->settings.github_access_token.empty())
        {
            device_authorization();
        }

        auto pull_requests_url = cpr::Url{"https://api.github.com/repos/" + results[1].str() + "/pulls"};
        auto pull_requests_header =
            cpr::Header{{"User-Agent", "Fons-Git-Client"}, {"Authorization", "token " + app->settings.github_access_token}};
        auto pull_requests_json = http_get(pull_requests_url, pull_requests_header);

        if (!pull_requests_json.has_value() || !(*pull_requests_json).is_array())
            return;

        for (auto current_pr : *pull_requests_json)
        {
            queue_event<pull_request_event>(EVENT_FOUND_PULL_REQUEST, wxID_ANY, current_pr["title"], current_pr["url"],
                                            current_pr["user"]["login"]);
        }
    }
} // namespace fons::git