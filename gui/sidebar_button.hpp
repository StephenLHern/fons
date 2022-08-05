#pragma once

#include <mutex>

#include "gui/fons_wx.hpp"

namespace fons::gui
{
    class sidebar_button : public wxButton
    {
      public:
        sidebar_button(wxWindow *parent, wxWindowID id, const wxString &label, const wxString &image);
        void mark_active();
        void mark_inactive();

      private:
        void on_mouse_enter(wxMouseEvent &evt);
        void on_mouse_leave(wxMouseEvent &evt);
        void load_image();
        wxString loaded_image;
    };
} // namespace fons::gui