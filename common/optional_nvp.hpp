#pragma once

// From Pavel Boev
// https://github.com/LowCostCustoms/cereal-optional

#include "cereal/cereal.hpp"

namespace cereal
{

    /// @struct OptionalNameValuePair
    /// @brief Name-value pair that can be default-initialized in case of name search failure
    /// @tparam T Property type
    /// @tparam TV Property value type
    template <class T, class TV>
    struct OptionalNameValuePair
    {
        // same as the cereal::NameValuePair::Type cause it's private
        using Type =
            typename std::conditional_t<std::is_array_v<typename std::remove_reference_t<T>>, typename std::remove_cv_t<T>,
                                        typename std::conditional_t<std::is_lvalue_reference_v<T>, T, typename std::decay_t<T>>>;

        // source property
        Type value;

        // default property value
        TV defaultValue;

        // property name
        const char *name;

        OptionalNameValuePair(const char *name, T &&value, TV &&defaultValue)
            : name(name), value(std::move<T>(value)), defaultValue(std::move<TV>(defaultValue))
        {
        }

        /// Delete default copy constructor
        OptionalNameValuePair(const OptionalNameValuePair &) = delete;

        /// And enable move constructor by default
        OptionalNameValuePair(OptionalNameValuePair &&) noexcept = default;
    };

    template <class T>
    struct OptionalNameValuePair<T, void>
    {
        // same as the cereal::NameValuePair::Type cause it's private
        using Type =
            typename std::conditional_t<std::is_array_v<typename std::remove_reference_t<T>>, typename std::remove_cv_t<T>,
                                        typename std::conditional_t<std::is_lvalue_reference_v<T>, T, typename std::decay_t<T>>>;

        // source property
        Type value;

        // property name
        const char *name;

        OptionalNameValuePair(const char *name, T &&value) : value(value), name(name)
        {
        }

        /// Delete default copy constructor
        OptionalNameValuePair(const OptionalNameValuePair &) = delete;

        /// And enable move constructor by default
        OptionalNameValuePair(OptionalNameValuePair &&) noexcept = default;
    };

    /// Makes optional name-value pair
    /// @param name Property name
    /// @param option Property
    /// @param defaultValue Default property value
    /// @return Optional NVP
    template <class T, class TV>
    OptionalNameValuePair<T, TV> make_optional_nvp(const char *name, T &&option, TV &&defaultValue)
    {
        return {name, std::forward<T>(option), std::forward<TV>(defaultValue)};
    };
    template <class T>
    OptionalNameValuePair<T, void> make_optional_nvp(const char *name, T &&option)
    {
        return {name, std::forward<T>(option)};
    };

    /// Optional name-value pair builder method overload for std::string property name
    template <class T, class TV>
    OptionalNameValuePair<T, TV> make_optional_nvp(const std::string &name, T &&option, TV &&defaultValue)
    {
        return {name.c_str(), std::forward<T>(option), std::forward<TV>(defaultValue)};
    };
    template <class T>
    OptionalNameValuePair<T, void> make_optional_nvp(const std::string &name, T &&option)
    {
        return {name.c_str(), std::forward<T>(option)};
    };
} // namespace cereal

#define CEREAL_OPTIONAL_NVP(T, ...) ::cereal::make_optional_nvp(#T, T, __VA_ARGS__)