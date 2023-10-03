/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_core/juce_core.h>

#include "../pybind11/embed.h"

#include <functional>
#include <typeinfo>

//=================================================================================================

namespace straw::Bindings {

void registerJuceCoreBindings (pybind11::module_& m);

} // namespace straw::Bindings

//=================================================================================================

namespace PYBIND11_NAMESPACE {
namespace detail {

template <>
struct type_caster<juce::String>
{
public:
    PYBIND11_TYPE_CASTER (juce::String, const_name ("String"));

    bool load (handle src, bool convert);

    static handle cast (const juce::String& src, return_value_policy policy, handle parent);
};

template <>
struct type_caster<juce::StringRef>
{
public:
    PYBIND11_TYPE_CASTER (juce::StringRef, const_name ("StringRef"));

    bool load (handle src, bool convert);

    static handle cast (const juce::StringRef& src, return_value_policy policy, handle parent);
};

template <>
struct type_caster<juce::Identifier>
{
public:
    PYBIND11_TYPE_CASTER (juce::Identifier, const_name ("Identifier"));

    bool load (handle src, bool convert);

    static handle cast (const juce::Identifier& src, return_value_policy policy, handle parent);
};

template <>
struct type_caster<juce::var>
{
public:
    PYBIND11_TYPE_CASTER (juce::var, const_name ("var"));

    bool load (handle src, bool convert);

    static handle cast (const juce::var& src, return_value_policy policy, handle parent);
};

template <>
struct type_caster<juce::StringArray>
{
public:
    PYBIND11_TYPE_CASTER (juce::StringArray, const_name ("StringArray"));

    bool load (handle src, bool convert);

    static handle cast (const juce::StringArray& src, return_value_policy policy, handle parent);
};

template <>
struct type_caster<juce::NamedValueSet>
{
public:
    PYBIND11_TYPE_CASTER (juce::NamedValueSet, const_name ("NamedValueSet[Identifier, var]"));

    bool load (handle src, bool convert);

    static handle cast (const juce::NamedValueSet& src, return_value_policy policy, handle parent);
};

} // namespace detail
} // namespace PYBIND11_NAMESPACE
