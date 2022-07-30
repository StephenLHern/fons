#include "app_main.hpp"
#include "events/git_found_remote_event.hpp"
#include "git/commands/find_branches.hpp"
#include "git/commands/find_repos.hpp"
#include "git/commands/revwalk.hpp"
#include "git/commands/status.hpp"
#include "gui/frame_home.hpp"

namespace fons
{
    wxIMPLEMENT_APP(app_main);

    bool app_main::OnInit()
    {
        cmd_manager.init(this, &settings);
        git_mediator.emplace(&settings, &cmd_manager);

        gui::frame_home *frame = new gui::frame_home(this);
        frame->Show(true);

        Bind(git::EVENT_FOUND_REPO, &app_settings::on_repo_found, &settings);
        Bind(git::EVENT_INIT_REPO, &app_settings::on_repo_select, &settings);
        Bind(git::EVENT_FOUND_BRANCH, &git::git_mediator::on_branch_found, &(git_mediator.value()));
        Bind(git::EVENT_REVWALK, &git::git_mediator::on_commit_found, &(git_mediator.value()));
        Bind(git::EVENT_STATUS, &git::git_mediator::on_status, &(git_mediator.value()));
        Bind(events::EVENT_GIT_FOUND_REMOTE, &git::git_mediator::on_remote_found, &(git_mediator.value()));
        Bind(events::EVENT_CMD_COMPLETE, &app_cmd_manager::on_cmd_complete, &cmd_manager);

        git_mediator->init();
        return true;
    }

    int app_main::OnExit()
    {
        return 0;
    }

    void app_main::on_close(wxCloseEvent &event_data)
    {
        cmd_manager.shutdown();
        event_data.Skip();
    }
} // namespace fons