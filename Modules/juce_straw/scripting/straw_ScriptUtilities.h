/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include "pybind11/embed.h"

#include <optional>

namespace straw {

/**
 * @brief
 */
template <class T>
std::optional<T> python_cast (const pybind11::object& value)
{
    try
    {
        return value.cast<T>();
    }
    catch (const pybind11::cast_error& e)
    {
        return std::nullopt;
    }
}

} // namespace straw
