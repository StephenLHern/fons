#include <wx/stc/stc.h>

#include "app_cmd_manager.hpp"
#include "app_settings.hpp"
#include "context_changes.hpp"
#include "git/commands/head_to_workdir.hpp"
#include "git/git_mediator.hpp"

namespace fons::gui
{
    context_changes::context_changes(wxWindow *parent, fons::app_settings &bound_settings, git::git_mediator &bound_git,
                                     app_cmd_manager &cmd_manager)
        : wxPanel(parent)
    {
        associated_cmd_manager = &cmd_manager;

        bound_settings.subscribe(this);
        subscribed_settings = &bound_settings;

        bound_git.subscribe(this);
        subscribed_git = &bound_git;

        wxBoxSizer *file_list_sizer = new wxBoxSizer(wxOrientation::wxVERTICAL);

        SetSizerAndFit(file_list_sizer);
    }

    void context_changes::on_command_complete([[maybe_unused]] uint64_t command_id)
    {

        if (!last_head_to_workdir_cmd || command_id != last_head_to_workdir_cmd->id())
            return;

        DestroyChildren();
        GetSizer()->Clear();

        for (git::file_diff current_file : last_head_to_workdir_cmd->modified_file_list)
        {
            // TODO: Replace text controls with a, yet to be created, customized control for displaying file diffs, to handle syntax
            // coloring, diff highlighting, unified scrolling, line numbers, editing

            wxFont file_label_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

            auto *old_file_label =
                new wxStaticText(this, wxID_ANY, current_file.old_file_path, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
            old_file_label->SetFont(file_label_font);

            auto old_file_text = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
            old_file_text->SetSTCCursor(wxSTC_CURSORARROW);
            old_file_text->SetCaretStyle(wxSTC_CARETSTYLE_INVISIBLE);
            old_file_text->SetUseHorizontalScrollBar(false);
            old_file_text->SetMarginWidth(1, 0);
            old_file_text->SetText(current_file.old_file_content);

            auto *old_file_sizer = new wxBoxSizer(wxOrientation::wxVERTICAL);
            old_file_sizer->Add(old_file_label);
            old_file_sizer->Add(old_file_text, wxSizerFlags(1).Expand());

            auto *new_file_label =
                new wxStaticText(this, wxID_ANY, current_file.new_file_path, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
            new_file_label->SetFont(file_label_font);

            auto new_file_text = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
            new_file_text->SetSTCCursor(wxSTC_CURSORARROW);
            new_file_text->SetCaretStyle(wxSTC_CARETSTYLE_INVISIBLE);
            new_file_text->SetUseHorizontalScrollBar(false);
            new_file_text->SetMarginWidth(1, 0);
            new_file_text->SetText(current_file.new_file_conent);

            auto *new_file_sizer = new wxBoxSizer(wxOrientation::wxVERTICAL);
            new_file_sizer->Add(new_file_label);
            new_file_sizer->Add(new_file_text, wxSizerFlags(1).Expand());

            auto *file_diff_spacer = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
            file_diff_spacer->AddSpacer(5);
            file_diff_spacer->Add(old_file_sizer, wxSizerFlags(1).Expand());
            file_diff_spacer->AddSpacer(5);
            file_diff_spacer->Add(new_file_sizer, wxSizerFlags(1).Expand());
            file_diff_spacer->AddSpacer(5);

            GetSizer()->Add(file_diff_spacer, wxSizerFlags(1).Expand());
        }

        Layout();
    }

    void context_changes::on_repo_select(std::string found_repo)
    {
        last_head_to_workdir_cmd.reset();
        auto head_to_workdir_cmd = std::make_shared<fons::git::head_to_workdir>();
        last_head_to_workdir_cmd = head_to_workdir_cmd;
        head_to_workdir_cmd->subscribe(this);
        associated_cmd_manager->execute(head_to_workdir_cmd);
    }

} // namespace fons::gui