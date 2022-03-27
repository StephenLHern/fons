#include "context_remote.hpp"

namespace fons::gui
{
    context_remote::context_remote(wxWindow *parent) : wxPanel(parent)
    {
        auto *remote_placeholder = new wxTextCtrl(this, wxID_ANY, "Remote (to be implemented)");

        auto *context_horizontal = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        context_horizontal->AddSpacer(5);
        context_horizontal->Add(remote_placeholder, wxSizerFlags(1).Expand());

        auto *context_vertical = new wxBoxSizer(wxOrientation::wxVERTICAL);
        context_vertical->AddSpacer(5);
        context_vertical->Add(context_horizontal, wxSizerFlags(1).Expand());

        SetSizerAndFit(context_vertical);
    }
} // namespace fons::gui