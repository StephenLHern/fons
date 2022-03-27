#include "git_mediator.hpp"
#include "app_cmd_manager.hpp"
#include "app_settings.hpp"

#include "git/commands/find_branches.hpp"
#include "git/commands/find_repos.hpp"
#include "git/commands/revwalk.hpp"
#include "git/commands/status.hpp"

namespace fons::git
{
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

        auto init_branches_cmd = std::make_shared<fons::git::find_branches>();
        cmd_manager->execute(init_branches_cmd);
        last_find_branches_cmd = init_branches_cmd;

        while (init_branches_cmd->status != common::cmd_status::joined)
            std::this_thread::yield();

        auto init_status_cmd = std::make_shared<fons::git::status>();
        cmd_manager->execute(init_status_cmd);
    }

    void git_mediator::on_branch_found(wxCommandEvent &eventData)
    {
        std::string found_branch = std::string(eventData.GetString());

        for (git_observer *current_observer : observers)
            current_observer->on_branch_found(found_branch);
    }

    void git_mediator::on_commit_found(revwalk_event &eventData)
    {
        for (git_observer *current_observer : observers)
            current_observer->on_commit_found(eventData.commit_data);
    }

    void git_mediator::on_status(wxCommandEvent &eventData)
    {
        std::string active_branch = std::string(eventData.GetString());

        if (cached_active_branch == active_branch)
            return;

        cached_active_branch = active_branch;

        for (git_observer *current_observer : observers)
            current_observer->on_status_change(active_branch);

        auto revwalk = std::make_shared<fons::git::revwalk>();
        cmd_manager->execute(revwalk);
        last_revwalk_cmd = revwalk;
    }

    void fons::git::git_mediator::on_repo_select(std::string selected_repo)
    {
        cached_active_branch.clear();

        std::shared_ptr<fons::git::find_branches> lapsed_branch_cmd = last_find_branches_cmd.lock();
        if (lapsed_branch_cmd)
        {
            lapsed_branch_cmd->cancel();
        }

        auto init_branches_cmd = std::make_shared<fons::git::find_branches>();
        cmd_manager->execute(init_branches_cmd);
        last_find_branches_cmd = init_branches_cmd;

        while (init_branches_cmd->status != common::cmd_status::joined)
            std::this_thread::yield();

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