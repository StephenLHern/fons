#include "git_mediator.hpp"
#include "app_cmd_manager.hpp"
#include "app_settings.hpp"

#include "events/git_found_remote_event.hpp"
#include "git/commands/find_branches.hpp"
#include "git/commands/find_pull_requests.hpp"
#include "git/commands/find_remotes.hpp"
#include "git/commands/find_repos.hpp"
#include "git/commands/get_config.hpp"
#include "git/commands/revwalk.hpp"
#include "git/commands/status.hpp"
#include "git/commit.hpp"

namespace fons::git
{
    wxDEFINE_EVENT(EVENT_INIT_REPO, wxCommandEvent);

    git_mediator::git_mediator(fons::app_settings *bind_settings, fons::app_cmd_manager *bind_cmd_manager)
        : settings(bind_settings), cmd_manager(bind_cmd_manager)
    {
        settings->subscribe(this);
    }

    git_mediator::~git_mediator()
    {
        settings->unsubscribe(this);
    }

    void git_mediator::subscribe(git_observer *observer)
    {
        observers.push_back(observer);
    }

    void git_mediator::unsubscribe(git_observer *observer)
    {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void git_mediator::init()
    {
        std::chrono::hours repo_search_ellapsed =
            std::chrono::duration_cast<std::chrono::hours>(settings->last_repo_scan_time - std::chrono::system_clock::now());

        if (repo_search_ellapsed < std::chrono::hours(0) || repo_search_ellapsed > std::chrono::hours(2))
        {
            auto init_repos_cmd = std::make_shared<fons::git::find_repos>();
            init_repos_cmd->subscribe(this);
            cmd_manager->execute(init_repos_cmd);
            last_find_repos_cmd = init_repos_cmd;
        }

        if (!settings->active_repo.empty())
        {
            auto repo_select_event = new wxCommandEvent(EVENT_INIT_REPO);
            repo_select_event->SetString(settings->active_repo);
            wxQueueEvent(dynamic_cast<wxEvtHandler *>(settings), repo_select_event);
        }
    }

    void git_mediator::on_branch_found(const wxCommandEvent &eventData)
    {
        for (git_observer *current_observer : observers)
            current_observer->on_branch_found(std::string_view(eventData.GetString()));
    }

    void git_mediator::on_commit_found(const revwalk_event &eventData)
    {
        for (git_observer *current_observer : observers)
            current_observer->on_commit_found(eventData.commit_data);
    }

    void git_mediator::on_remote_found(const events::git_found_remote_event &eventData)
    {
        for (git_observer *current_observer : observers)
            current_observer->on_remote_found(eventData.remote_name, eventData.remote_url);

        auto find_pull_requests_for_remote = std::make_shared<fons::git::find_pull_requests>();
        find_pull_requests_for_remote->remote_url = eventData.remote_url;
        cmd_manager->execute(find_pull_requests_for_remote);
    }

    void fons::git::git_mediator::on_pull_request_found(const pull_request_event &eventData)
    {
        for (git_observer *current_observer : observers)
            current_observer->on_pull_request_found(eventData.title, eventData.url, eventData.user_login);
    }

    void git_mediator::on_status(const wxCommandEvent &eventData)
    {
        auto active_branch = std::string(eventData.GetString());

        if (cached_active_branch == active_branch)
            return;

        cached_active_branch = active_branch;

        for (git_observer *current_observer : observers)
            current_observer->on_status_change(active_branch);

        auto revwalk = std::make_shared<fons::git::revwalk>();
        cmd_manager->execute(revwalk);
        last_revwalk_cmd = revwalk;
    }

    void fons::git::git_mediator::on_repo_select([[maybe_unused]] std::string_view selected_repo)
    {
        cached_active_branch.clear();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // A new repository has been selected, so cancel any executing commands operating on the previous repository
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if (std::shared_ptr<fons::git::find_branches> lapsed_branch_cmd = last_find_branches_cmd.lock(); lapsed_branch_cmd)
        {
            lapsed_branch_cmd->cancel();
        }

        if (std::shared_ptr<fons::git::find_remotes> lapsed_remotes_cmd = last_find_remotes_cmd.lock(); lapsed_remotes_cmd)
        {
            lapsed_remotes_cmd->cancel();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Queue new commands to find relevant details about the newly selected repository
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        auto init_remotes_cmd = std::make_shared<fons::git::find_remotes>();
        cmd_manager->execute(init_remotes_cmd);
        last_find_remotes_cmd = init_remotes_cmd;

        auto init_branches_cmd = std::make_shared<fons::git::find_branches>();
        cmd_manager->execute(init_branches_cmd);
        last_find_branches_cmd = init_branches_cmd;

        auto init_get_config_cmd = std::make_shared<fons::git::get_config>();
        cmd_manager->execute(init_get_config_cmd);
        last_get_config_cmd = init_get_config_cmd;

        auto init_status_cmd = std::make_shared<fons::git::status>();
        cmd_manager->execute(init_status_cmd);
    }

    void git_mediator::on_command_complete(uint64_t command_id)
    {
        std::shared_ptr<find_repos> find_repos_cmd = last_find_repos_cmd.lock();
        if (find_repos_cmd && command_id == find_repos_cmd->id())
        {
            settings->last_repo_scan_time = std::chrono::system_clock::now();
        }
    }
} // namespace fons::git