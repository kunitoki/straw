
/**
 * juce python - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include "../pybind11/embed.h"

namespace jucepy::Helpers {

//=================================================================================================

template <class T>
struct CppToPython;

template <>
struct CppToPython<int>
{
    using type = pybind11::int_;
};

template <>
struct CppToPython<float>
{
    using type = pybind11::float_;
};

} // namespace jucepy::Helpers
