#pragma once

// From Pavel Boev
// https://github.com/LowCostCustoms/cereal-optional

#include "cereal/archives/json.hpp"
#include "optional_nvp.hpp"

namespace cereal
{

    template <class T, class TV>
    void prologue(JSONInputArchive &, const OptionalNameValuePair<T, TV> &)
    {
        // Prologue, epilogue functions required for cereal registration
    }

    template <class T, class TV>
    void epilogue(JSONInputArchive &, const OptionalNameValuePair<T, TV> &)
    {
        // Prologue, epilogue functions required for cereal registration
    }

    template <class T, class TV>
    void prologue(JSONOutputArchive &, const OptionalNameValuePair<T, TV> &)
    {
        // Prologue, epilogue functions required for cereal registration
    }

    template <class T, class TV>
    void epilogue(JSONOutputArchive &, const OptionalNameValuePair<T, TV> &)
    {
        // Prologue, epilogue functions required for cereal registration
    }

    /// Loads optional nvp from JSONInputArchive if any. In case of exception sets fallback value
    /// @param archive JSONInputArchive
    /// @param nvp Optional NVP to load
    template <class T, class TV>
    void CEREAL_LOAD_FUNCTION_NAME(JSONInputArchive &archive, OptionalNameValuePair<T, TV> &nvp)
    {
        try
        {
            archive.setNextName(nvp.name);
            archive(nvp.value);
        }
        catch (cereal::Exception &)
        {
            if constexpr (!std::is_same_v<TV, void>)
            {
                nvp.value = std::move(nvp.defaultValue);
            }
            archive.setNextName(nullptr);
        }
    }

    /// Saves optional nvp to JSONOutputArchive in standard way like NameValuePair
    /// @param archive JSONOutputArchive
    /// @param nvp Optional NVP to save
    template <class T, class TV>
    void CEREAL_SAVE_FUNCTION_NAME(JSONOutputArchive &archive, OptionalNameValuePair<T, TV> const &nvp)
    {
        archive.setNextName(nvp.name);
        archive(nvp.value);
    }

} // namespace cereal