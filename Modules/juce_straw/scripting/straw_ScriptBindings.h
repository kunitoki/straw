/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "pybind11/embed.h"

#include <functional>
#include <typeinfo>

namespace straw {

//=================================================================================================

using ComponentTypeCaster = std::function<const void* (const juce::Component*, const std::type_info*&)>;

//=================================================================================================

namespace Bindings {

struct ComponentTypeMap
{
    juce::CriticalSection mutex;
    std::unordered_map<juce::String, straw::ComponentTypeCaster> typeMap;
};

ComponentTypeMap& getComponentTypeMap();
void registerComponentType (juce::StringRef className, ComponentTypeCaster classCaster);
void clearComponentTypes();

} // namespace

//=================================================================================================

template <class T>
const void* ComponentType(const juce::Component* src, const std::type_info*& type)
{
    static_assert (std::is_base_of_v<juce::Component, T>, "Invalid unrelated polymorphism between classes");

    type = &typeid(T);
    return dynamic_cast<const T*> (src);
}

} // namespace straw
