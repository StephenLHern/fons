#pragma once

#include "gui/fons_wx.h"

namespace fons::gui
{
    class everything_but_the_title_bar : public wxPanel
    {
      public:
        everything_but_the_title_bar(wxPanel *parent, wxApp *app, int id = wxID_ANY);
        wxPanel *m_parent;

      private:
        wxWindow *m_drag_window;
        wxPoint m_dragStartMouse;
        wxPoint m_dragStartWindow;
        bool m_dragging;
        void on_size(wxSizeEvent &event);
        void on_mouse_left_down(wxMouseEvent &event);
        void on_mouse_left_up(wxMouseEvent &event);
        void on_mouse_motion(wxMouseEvent &event);
        void on_mouse_capture_lost(wxMouseCaptureLostEvent &event);
        void finish_drag();
    };
} // namespace fons::gui