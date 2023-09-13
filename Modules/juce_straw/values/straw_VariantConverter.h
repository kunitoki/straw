/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace straw {

//=================================================================================================

template <class T>
struct VariantConverter
{
    static T fromVar(const juce::var& v)
    {
        return static_cast<T>(v);
    }
    
    static juce::var toVar(const T& v)
    {
        return juce::var(v);
    }
};

//=================================================================================================

template <class T>
struct VariantConverter<juce::Point<T>>
{
    static juce::Point<T> fromVar(const juce::var& v)
    {
        jassert (v.isObject());
        return { v.getProperty("x", 0), v.getProperty("y", 0) };
    }

    static juce::var toVar(const juce::Point<T>& v)
    {
        juce::DynamicObject::Ptr object = new juce::DynamicObject;
        object->setProperty("x", v.getX());
        object->setProperty("y", v.getX());
        return object.get();
    }
};

//=================================================================================================

template <class T>
struct VariantConverter<juce::Rectangle<T>>
{
    static juce::Rectangle<T> fromVar(const juce::var& v)
    {
        jassert (v.isObject());
        return { v.getProperty("x", 0), v.getProperty("y", 0), v.getProperty("width", 0), v.getProperty("height", 0) };
    }

    static juce::var toVar(const juce::Rectangle<T>& v)
    {
        juce::DynamicObject::Ptr object = new juce::DynamicObject;
        object->setProperty("x", v.getX());
        object->setProperty("y", v.getX());
        object->setProperty("width", v.getWidth());
        object->setProperty("height", v.getHeight());
        return object.get();
    }
};

//=================================================================================================

template <>
struct VariantConverter<juce::AffineTransform>
{
    static juce::AffineTransform fromVar(const juce::var& v)
    {
        jassert (v.isObject());
        return {
            v.getProperty("mat00", 0),
            v.getProperty("mat01", 0),
            v.getProperty("mat02", 0),
            v.getProperty("mat10", 0),
            v.getProperty("mat11", 0),
            v.getProperty("mat12", 0)
        };
    }

    static juce::var toVar(const juce::AffineTransform& v)
    {
        juce::DynamicObject::Ptr object = new juce::DynamicObject;
        object->setProperty("mat00", v.mat00);
        object->setProperty("mat01", v.mat01);
        object->setProperty("mat02", v.mat02);
        object->setProperty("mat10", v.mat10);
        object->setProperty("mat11", v.mat11);
        object->setProperty("mat12", v.mat12);
        return object.get();
    }
};

//=================================================================================================

template <>
struct VariantConverter<juce::NamedValueSet>
{
    static juce::NamedValueSet fromVar(const juce::var& v)
    {
        jassert (v.isObject());

        juce::NamedValueSet result;

        if (auto object = v.getDynamicObject())
            result = object->getProperties();
        
        return result;
    }

    static juce::var toVar(const juce::NamedValueSet& v)
    {
        juce::DynamicObject::Ptr object = new juce::DynamicObject;

        for (const auto& nameValue : v)
            object->setProperty(nameValue.name, nameValue.value);

        return object.get();
    }
};

//=================================================================================================

template <class T>
[[nodiscard]] T fromVar (const juce::var& v)
{
    return VariantConverter<T>::fromVar (v);
}

template <class T>
[[nodiscard]] juce::var toVar (const T& v)
{
    return VariantConverter<T>::toVar (v);
}

} // namespace straw
