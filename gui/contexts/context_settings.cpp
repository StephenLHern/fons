#include <ranges>
#include <wx/treectrl.h>

#include "app_cmd_manager.hpp"
#include "app_settings.hpp"
#include "context_settings.hpp"
#include "git/commands/get_config.hpp"
#include "git/git_mediator.hpp"

namespace fons::gui
{
    context_settings::context_settings(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git,
                                       app_cmd_manager &cmd_manager)
        : wxPanel(parent)
    {
        associated_cmd_manager = &cmd_manager;

        bound_settings.subscribe(this);
        subscribed_settings = &bound_settings;

        bound_git.subscribe(this);
        subscribed_git = &bound_git;

        config_tree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, (wxBORDER_NONE | wxTR_HIDE_ROOT));

        wxBoxSizer *tree_sizer = new wxBoxSizer(wxOrientation::wxVERTICAL);
        tree_sizer->Add(config_tree, wxSizerFlags(1).Expand());
        SetSizerAndFit(tree_sizer);
    }

    void context_settings::on_command_complete(uint64_t command_id)
    {
        if (!last_get_config_cmd || command_id != last_get_config_cmd->id())
            return;

        // Temporarily freeze window drawing while processsing changes
        Freeze();

        config_tree->DeleteAllItems();
        config_tree->AddRoot("Configuration");

        const auto range_to_string_view = [](auto &&range)
        { return std::string_view(&*range.begin(), static_cast<size_t>(std::ranges::distance(range))); };

        for (git::config_entry current_entry : last_get_config_cmd->config_entries)
        {
            std::string path = current_entry.name.substr(0, current_entry.name.find_last_of('.'));

            wxTreeItemId last_node = config_tree->GetRootItem();
            for (auto current_path : path | std::ranges::views::split('.') | std::ranges::views::transform(range_to_string_view))
            {
                bool node_already_exists = false;
                wxTreeItemIdValue cookie;

                for (wxTreeItemId search_node = config_tree->GetFirstChild(last_node, cookie); search_node.IsOk();
                     search_node = config_tree->GetNextChild(last_node, cookie))
                {
                    if (current_path.compare(config_tree->GetItemText(search_node)) == 0)
                    {
                        node_already_exists = true;
                        last_node = search_node;
                        break;
                    }
                }

                if (!node_already_exists)
                    last_node = config_tree->AppendItem(last_node, std::string(current_path));
            }

            std::string config = current_entry.name.substr(current_entry.name.find_last_of('.') + 1, current_entry.name.size());
        }

        config_tree->ExpandAll();
        Layout();

        // Unfreeze window drawing now that all changes have been processed
        Thaw();
    }

    void context_settings::on_repo_select(std::string found_repo)
    {
        last_get_config_cmd.reset();
        auto get_config_cmd = std::make_shared<fons::git::get_config>();
        last_get_config_cmd = get_config_cmd;
        last_get_config_cmd->subscribe(this);
        associated_cmd_manager->execute(last_get_config_cmd);
    }
} // namespace fons::gui