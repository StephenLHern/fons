#include <wx/stc/stc.h>

#include "context_info.hpp"

namespace fons::gui
{
    context_info::context_info(wxWindow *parent) : wxPanel(parent)
    {
        auto *license_info = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        license_info->LoadFile("license_notices.txt");
        license_info->SetReadOnly(true);
        license_info->SetSTCCursor(wxSTC_CURSORARROW);
        license_info->SetCaretStyle(wxSTC_CARETSTYLE_INVISIBLE);
        license_info->SetUseHorizontalScrollBar(false);
        license_info->SetMarginWidth(1, 0);

        auto *context_horizontal = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        context_horizontal->AddSpacer(5);
        context_horizontal->Add(license_info, wxSizerFlags(1).Expand());

        auto *context_vertical = new wxBoxSizer(wxOrientation::wxVERTICAL);
        context_vertical->AddSpacer(5);
        context_vertical->Add(context_horizontal, wxSizerFlags(1).Expand());

        SetSizerAndFit(context_vertical);
    }
} // namespace fons::gui