#pragma once

#include <wx/simplebook.h>

#include "gui/fons_wx.h"

namespace fons
{
    class app_main;
}

namespace fons::gui
{
    class frame_home : public wxFrame
    {
      public:
        frame_home(wxApp *parent);

      private:
        app_main *parent_app;
        wxSimplebook *context_book;
        void on_sidebar_select(wxCommandEvent &event_data);
        std::unordered_map<wxWindowID, size_t> button_id_to_page_id;
        WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override;
    };
} // namespace fons::gui