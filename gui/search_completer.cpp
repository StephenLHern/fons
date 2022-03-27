#include "search_completer.hpp"

namespace fons::gui
{
    search_completer::search_completer(wxArrayString searchValues)
    {
        for (wxString value : searchValues)
            search_cache.push_back(value);
    }

    void search_completer::GetCompletions(const wxString &prefix, wxArrayString &res)
    {
        std::string stl_prefix = std::string(prefix.mb_str());
        for (wxString cached_val : search_cache)
        {
            std::string stl_cache = std::string(cached_val.mb_str());
            if (stl_cache.find(stl_prefix) != std::string::npos)
            {
                res.push_back(cached_val);
            }
        }
    }
} // namespace fons::gui