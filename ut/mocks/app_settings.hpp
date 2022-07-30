#pragma once

#include <gmock/gmock.h>

#include "settings_observer.hpp"

class Archive;

namespace fons
{
    class app_settings
    {
      public:
        MOCK_METHOD(void, subscribe, (settings_observer * observer));
        MOCK_METHOD(void, unsubscribe, (settings_observer * observer));
        MOCK_METHOD(void, on_repo_select, (wxCommandEvent & eventData));
        MOCK_METHOD(void, on_repo_found, (wxCommandEvent & eventData));
        MOCK_METHOD(void, serialize, (Archive & archive));
        MOCK_METHOD(void, load_settings, ());
        MOCK_METHOD(void, save_settings, ());
        MOCK_METHOD(void, verify_settings, ());
    };
} // namespace fons