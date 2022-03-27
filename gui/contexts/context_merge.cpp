#include "context_merge.hpp"

namespace fons::gui
{
    context_merge::context_merge(wxWindow *parent) : wxPanel(parent)
    {
        auto *merge_placeholder = new wxTextCtrl(this, wxID_ANY, "Merge (to be implemented)");

        auto *context_horizontal = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        context_horizontal->AddSpacer(5);
        context_horizontal->Add(merge_placeholder, wxSizerFlags(1).Expand());

        auto *context_vertical = new wxBoxSizer(wxOrientation::wxVERTICAL);
        context_vertical->AddSpacer(5);
        context_vertical->Add(context_horizontal, wxSizerFlags(1).Expand());

        SetSizerAndFit(context_vertical);
    }
} // namespace fons::gui