#pragma once

#include "gui/fons_wx.hpp"

namespace fons::gui
{
    class everything_but_the_title_bar : public wxPanel
    {
      public:
        everything_but_the_title_bar(wxPanel *parent, wxApp *app, int id = wxID_ANY);

      private:
        wxWindow *m_drag_window;
        wxPoint m_dragStartMouse;
        wxPoint m_dragStartWindow;
        bool m_dragging;
        void on_size(const wxSizeEvent &event);
        void on_mouse_left_down(const wxMouseEvent &event);
        void on_mouse_left_up(const wxMouseEvent &event);
        void on_mouse_motion(const wxMouseEvent &event);
        void on_mouse_capture_lost(const wxMouseCaptureLostEvent &event);
        void finish_drag();
    };
} // namespace fons::gui