#include "wx/wx.h"

#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/mstream.h>
#include <wx/rawbmp.h>

#include <Vssym32.h>
#include <uxtheme.h>
#include <wx/msw/wrapwin.h>

#include <map>

namespace fons::gui
{
    class ebt_window_control_button : public wxButton
    {
      public:
        enum class button_type
        {
            Close = 0,
            Help,
            Max,
            Min,
            Restore
        };
        ebt_window_control_button(wxWindow *parent, wxWindowID id, button_type button_type);
        bool selected;

      private:
        button_type m_buttonType;
        wxWindow *m_top_parent;
        wxTopLevelWindow *m_top_window;
        int m_state;

        void on_mouse_enter(wxMouseEvent &evt);
        void on_mouse_leave(wxMouseEvent &evt);
        void on_mouse_left_down(wxMouseEvent &evt);
        void on_mouse_left_up(wxMouseEvent &evt);
        void on_size(wxSizeEvent &evt);
        void load_image();
    };
} // namespace fons::gui