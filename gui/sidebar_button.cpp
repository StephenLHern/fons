#include "sidebar_button.hpp"

namespace fons::gui
{
    sidebar_button::sidebar_button(wxWindow *parent, wxWindowID id, const wxString &label, const wxString &image)
        : wxButton(parent, id, label, wxDefaultPosition, wxSize(75, 50), (wxBORDER_NONE | wxBU_BOTTOM), wxDefaultValidator, label)
    {
        SetBackgroundColour(wxColour(243, 243, 243));
        loaded_image = image;
        load_image();

        Bind(wxEVT_ENTER_WINDOW, &sidebar_button::on_mouse_enter, this);
        Bind(wxEVT_LEAVE_WINDOW, &sidebar_button::on_mouse_leave, this);
    }
    void sidebar_button::mark_active()
    {
        if (loaded_image.Contains("-active"))
            return;

        loaded_image = loaded_image.insert(loaded_image.length() - 4, wxString("-active"));
        load_image();
        SetForegroundColour(wxColour(240, 80, 51));
    }
    void sidebar_button::mark_inactive()
    {
        int activeSpecifier = loaded_image.Find("-active");

        if (activeSpecifier < 0)
            return;

        loaded_image = loaded_image.Remove(static_cast<size_t>(activeSpecifier), 7);
        load_image();
        SetForegroundColour(*wxBLACK);
    }
    void sidebar_button::on_mouse_enter([[maybe_unused]] const wxMouseEvent &evt)
    {
        SetBackgroundColour(*wxWHITE);
    }
    void sidebar_button::on_mouse_leave([[maybe_unused]] const wxMouseEvent &evt)
    {
        SetBackgroundColour(wxColour(243, 243, 243));
    }
    void sidebar_button::load_image()
    {
        wxBitmap button_image;
        {
            // Some .png files cause warnings from their SRGB color space, these can be safely ignored
            wxLogNull logNo;
            button_image.LoadFile(loaded_image, wxBITMAP_TYPE_PNG);
        }
        wxFont sidebar_button_font(7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        SetBitmap(button_image);
        SetBitmapHover(button_image);
        SetBitmapDisabled(button_image);
        SetBitmapPressed(button_image);
        SetBitmapFocus(button_image);
        SetBitmapSelected(button_image);
        SetBitmapPosition(wxDirection::wxTOP);
        SetBitmapMargins(wxSize(20, 8));
        SetFont(sidebar_button_font);
    }
} // namespace fons::gui