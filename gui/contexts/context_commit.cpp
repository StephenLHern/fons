#include "context_commit.hpp"
#include "gui/diff_control.hpp"

namespace fons::gui
{
    context_commit::context_commit(wxWindow *parent) : wxPanel(parent)
    {
        // Create control
        auto *diff_panel = new fons::gui::diff_control(this);

        // Load files
        diff_panel->load_files("old.txt", "new.txt");

        // Customize display
        diff_panel->set_left_alias("Original");
        diff_panel->set_right_alias("Modified");
        diff_panel->set_left_editable(false);
        diff_panel->set_right_editable(true);

        // Set colors
        fons::gui::diff_colors colors;
        colors.added_lines = wxColour(200, 255, 200);
        diff_panel->set_diff_colors(colors);


        //auto *commit_placeholder = new wxTextCtrl(this, wxID_ANY, "Commit (to be implemented)");

        auto *context_horizontal = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        context_horizontal->AddSpacer(5);
        context_horizontal->Add(diff_panel, wxSizerFlags(1).Expand());
        //context_horizontal->Add(commit_placeholder, wxSizerFlags(1).Expand());

        auto *context_vertical = new wxBoxSizer(wxOrientation::wxVERTICAL);
        context_vertical->AddSpacer(5);
        context_vertical->Add(context_horizontal, wxSizerFlags(1).Expand());

        SetSizerAndFit(context_vertical);
    }
} // namespace fons::gui