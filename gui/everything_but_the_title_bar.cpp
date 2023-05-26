// Window Dragging Implementation based on code suggested by  New Pagodi
// https://stackoverflow.com/questions/69524059/how-to-create-control-buttons-exactly-like-windows-os-control-buttons-in-wxwi/69535224#69535224

#include "everything_but_the_title_bar.hpp"
#include "app_main.hpp"
#include "branch_selector.hpp"
#include "ebt_window_control_button.hpp"
#include "repo_selector.hpp"

namespace fons::gui
{
    everything_but_the_title_bar::everything_but_the_title_bar(wxPanel *parent, wxApp *app, int id)
        : wxPanel(parent, id, wxDefaultPosition, wxSize(-1, 30), wxBORDER_NONE)
    {
        Bind(wxEVT_LEFT_DOWN, &everything_but_the_title_bar::on_mouse_left_down, this);
        Bind(wxEVT_MOUSE_CAPTURE_LOST, &everything_but_the_title_bar::on_mouse_capture_lost, this);
        Bind(wxEVT_SIZE, &everything_but_the_title_bar::on_size, this);

        m_dragging = false;
        m_drag_window = GetParent()->GetParent();

        auto *repo_selector_label = new wxStaticText(this, wxID_ANY, "Repository:", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
        repo_selector_label->SetForegroundColour(*wxWHITE);

        wxFont navigation_label_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        repo_selector_label->SetFont(navigation_label_font);

        auto *parent_app = dynamic_cast<app_main *>(app);
        auto *home_repo_selector = new repo_selector(this, parent_app->settings);

        auto *repo_selector_sizer = new wxBoxSizer(wxOrientation::wxVERTICAL);
        repo_selector_sizer->Add(repo_selector_label, wxSizerFlags().FixedMinSize());
        repo_selector_sizer->Add(home_repo_selector, wxSizerFlags(1).Expand());

        auto *branch_selector_label = new wxStaticText(this, wxID_ANY, "Branch:", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
        branch_selector_label->SetForegroundColour(*wxWHITE);
        branch_selector_label->SetFont(navigation_label_font);

        auto *home_branch_selector = new branch_selector(this, parent_app->settings, parent_app->git_mediator.value());

        auto *branch_selector_sizer = new wxBoxSizer(wxOrientation::wxVERTICAL);
        branch_selector_sizer->Add(branch_selector_label, wxSizerFlags().FixedMinSize());
        branch_selector_sizer->Add(home_branch_selector, wxSizerFlags(1).Expand());

        auto *minimize_button = new ebt_window_control_button(this, wxID_ANY, ebt_window_control_button::button_type::Min);
        auto *maximize_button = new ebt_window_control_button(this, wxID_ANY, ebt_window_control_button::button_type::Max);
        auto *close_button = new ebt_window_control_button(this, wxID_ANY, ebt_window_control_button::button_type::Close);

        auto *status_controls_sizer = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        status_controls_sizer->AddSpacer(79);
        status_controls_sizer->Add(repo_selector_sizer, wxSizerFlags(1).Expand());
        status_controls_sizer->AddSpacer(14);
        status_controls_sizer->Add(branch_selector_sizer, wxSizerFlags(1).Expand());
        status_controls_sizer->AddSpacer(14);
        status_controls_sizer->SetMinSize(-1, 36);

        auto *status_controls_sizer_vertical = new wxBoxSizer(wxOrientation::wxVERTICAL);
        status_controls_sizer_vertical->AddSpacer(5);
        status_controls_sizer_vertical->Add(status_controls_sizer, wxSizerFlags(1).Expand());
        status_controls_sizer_vertical->AddSpacer(5);

        auto *title_bar_sizer = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        title_bar_sizer->Add(status_controls_sizer_vertical, wxSizerFlags(1).Expand());
        title_bar_sizer->Add(minimize_button, wxSizerFlags().FixedMinSize().Top());
        title_bar_sizer->Add(maximize_button, wxSizerFlags().FixedMinSize().Top());
        title_bar_sizer->Add(close_button, wxSizerFlags().FixedMinSize().Top());

        SetSizerAndFit(title_bar_sizer);
        SetBackgroundColour(wxColour(240, 80, 51));
    }

    void everything_but_the_title_bar::on_size([[maybe_unused]] const wxSizeEvent &event)
    {
        Layout();
    }

    void everything_but_the_title_bar::on_mouse_left_down(const wxMouseEvent &event)
    {
        if (!m_dragging)
        {
            Bind(wxEVT_LEFT_UP, &everything_but_the_title_bar::on_mouse_left_up, this);
            Bind(wxEVT_MOTION, &everything_but_the_title_bar::on_mouse_motion, this);
            m_dragging = true;

            wxPoint clientStart = event.GetPosition();
            m_dragStartMouse = ClientToScreen(clientStart);
            m_dragStartWindow = m_drag_window->GetPosition();
            CaptureMouse();
        }
    }

    void everything_but_the_title_bar::on_mouse_left_up([[maybe_unused]] const wxMouseEvent &event)
    {
        finish_drag();
    }

    void everything_but_the_title_bar::on_mouse_motion([[maybe_unused]] const wxMouseEvent &event)
    {
        wxPoint curClientPsn = event.GetPosition();
        wxPoint curScreenPsn = ClientToScreen(curClientPsn);
        wxPoint movementVector = curScreenPsn - m_dragStartMouse;

        m_drag_window->SetPosition(m_dragStartWindow + movementVector);
    }

    void everything_but_the_title_bar::on_mouse_capture_lost([[maybe_unused]] const wxMouseCaptureLostEvent &event)
    {
        finish_drag();
    }

    void everything_but_the_title_bar::finish_drag()
    {
        if (m_dragging)
        {
            Unbind(wxEVT_LEFT_UP, &everything_but_the_title_bar::on_mouse_left_up, this);
            Unbind(wxEVT_MOTION, &everything_but_the_title_bar::on_mouse_motion, this);
            m_dragging = false;
        }

        if (HasCapture())
        {
            ReleaseMouse();
        }
    }

} // namespace fons::gui