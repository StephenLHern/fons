#include "ebt_window_control_button.hpp"

namespace fons::gui
{
    ebt_window_control_button::ebt_window_control_button(wxWindow *parent, wxWindowID id, button_type button_type)
        : wxButton(parent, id, wxEmptyString, wxDefaultPosition, wxSize(45, 48), (wxBORDER_NONE))
    {
        selected = false;
        m_state = 1;
        m_buttonType = button_type;
        m_top_parent = this;
        while (m_top_parent->GetParent())
            m_top_parent = m_top_parent->GetParent();

        m_top_window = dynamic_cast<wxTopLevelWindow *>(m_top_parent);

        Bind(wxEVT_ENTER_WINDOW, &ebt_window_control_button::on_mouse_enter, this);
        Bind(wxEVT_LEAVE_WINDOW, &ebt_window_control_button::on_mouse_leave, this);
        Bind(wxEVT_LEFT_DOWN, &ebt_window_control_button::on_mouse_left_down, this);
        Bind(wxEVT_LEFT_UP, &ebt_window_control_button::on_mouse_left_up, this);
        Bind(wxEVT_SIZE, &ebt_window_control_button::on_size, this);

        switch (m_buttonType)
        {
        case button_type::Close:
        {
            SetId(wxID_CLOSE);
            break;
        }
        case button_type::Restore:
        case button_type::Max:
        {
            SetId(wxID_MAXIMIZE_FRAME);
            break;
        }
        case button_type::Min:
        {
            SetId(wxID_ICONIZE_FRAME);
            break;
        }
        case button_type::Help:
        {
            SetId(wxID_HELP_INDEX);
            break;
        }
        }

        load_image();
        SetBackgroundColour(wxColour(240, 80, 51));
    }

    void ebt_window_control_button::load_image()
    {
        if (m_buttonType == button_type::Restore || m_buttonType == button_type::Max)
            m_buttonType = (m_top_window->IsMaximized() ? button_type::Restore : button_type::Max);

        wxString to_load;
        switch (m_buttonType)
        {
        case button_type::Close:
        {
            to_load += "images/win-close";
            break;
        }
        case button_type::Max:
        {
            to_load += "images/win-maximize";
            break;
        }
        case button_type::Min:
        {
            to_load += "images/win-minimize";
            break;
        }
        case button_type::Restore:
        {
            to_load += "images/win-restore";
            break;
        }
        case button_type::Help:
        {
            to_load += "images/win-help";
            break;
        }
        }
        switch (m_state)
        {
        case 0:
        {
            to_load += "-inactive";
            break;
        }
        case 1:
        {
            to_load += "-active";
            break;
        }
        case 2:
        {
            to_load += "-selected";
            break;
        }
        }

        to_load += ".png";

        wxBitmap button_image;
        {
            // Some .png files cause warnings from their SRGB color space, these can be safely ignored
            wxLogNull logNo;
            button_image.LoadFile(to_load, wxBITMAP_TYPE_PNG);
        }
        SetBitmap(button_image);
        SetBitmapHover(button_image);
        SetBitmapDisabled(button_image);
        SetBitmapPressed(button_image);
        SetBitmapFocus(button_image);
        SetBitmapSelected(button_image);
    }

    void ebt_window_control_button::on_mouse_enter([[maybe_unused]] const wxMouseEvent &evt)
    {
        selected = true;
        m_state = 1;
        SetBackgroundColour((m_buttonType == button_type::Close) ? wxColour(232, 17, 35) : wxColour(238, 59, 27));
        load_image();
    }
    void ebt_window_control_button::on_mouse_leave([[maybe_unused]] const wxMouseEvent &evt)
    {
        selected = false;
        m_state = 1;
        SetBackgroundColour(wxColour(240, 80, 51));
        load_image();
    }
    void ebt_window_control_button::on_mouse_left_down([[maybe_unused]] const wxMouseEvent &evt)
    {
        m_state = 2;
        load_image();
    }
    void ebt_window_control_button::on_mouse_left_up([[maybe_unused]] const wxMouseEvent &evt)
    {
        m_state = 1;
        load_image();

        switch (m_buttonType)
        {
        case button_type::Close:
        {
            m_top_parent->Close();
            break;
        }
        case button_type::Max:
        {
            m_top_window->Maximize(true);
            break;
        }
        case button_type::Min:
        {
            m_top_window->Iconize(true);
            break;
        }
        case button_type::Restore:
        {
            m_top_window->Maximize(false);
            break;
        }
        case button_type::Help:
        {
            break;
        }
        }
    }
    void ebt_window_control_button::on_size([[maybe_unused]] const wxSizeEvent &evt)
    {
        load_image();
    }
} // namespace fons::gui