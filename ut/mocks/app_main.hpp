#include <gmock/gmock.h>

#include "settings_observer.hpp"

class Archive;

namespace fons
{
    class app_main : public wxApp
    {
      public:
        MOCK_METHOD(bool, OnInit, ());
        MOCK_METHOD(int, OnExit, ());
        MOCK_METHOD(void, on_close, (wxCloseEvent & event_data));
    };
} // namespace fons