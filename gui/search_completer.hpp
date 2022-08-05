#pragma once

#include <wx/textcompleter.h>

#include "gui/fons_wx.hpp"

namespace fons::gui
{
    class search_completer : public wxTextCompleterSimple
    {
      public:
        search_completer(wxArrayString searchValues);
        virtual void GetCompletions(const wxString &prefix, wxArrayString &res) override;

      private:
        wxArrayString search_cache;
    };
} // namespace fons::gui