/*
  ==============================================================================

   This file is part of the straw project.
   Copyright (c) 2024 - kunitoki@gmail.com

   straw is an open source library subject to open-source licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   STRAW IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#include "straw_ComponentHelpers.h"

#include "../values/straw_VariantConverter.h"

namespace straw::Helpers {

//=================================================================================================

juce::Component* findComponentById (juce::Component* component, juce::StringRef id)
{
    if (component->getComponentID() == id)
        return component;

    if (auto child = component->findChildWithID (id))
        return child;

    for (int i = 0; i < component->getNumChildComponents(); ++i)
    {
        auto child = component->getChildComponent (i);
        if (child == nullptr)
            continue;

        if (auto foundComponent = findComponentById (child, id))
            return foundComponent;
    }

    return nullptr;
}

juce::Component* findComponentById (juce::StringRef id)
{
    for (int i = 0; i < juce::Desktop::getInstance().getNumComponents(); ++i)
    {
        auto component = findComponentById (juce::Desktop::getInstance().getComponent (i), id);
        if (component != nullptr)
            return component;
    }

    return nullptr;
}

//=================================================================================================

juce::Array<juce::Component*> findComponentsByType (juce::Component* component, juce::StringRef typeName)
{
    juce::Array<juce::Component*> result;

    if (popsicle::Helpers::demangleClassName (typeid (*component).name()) == typeName)
        result.addIfNotAlreadyThere (component);

    for (int i = 0; i < component->getNumChildComponents(); ++i)
    {
        if (auto child = component->getChildComponent (i); popsicle::Helpers::demangleClassName (typeid (*child).name()) == typeName)
            result.addIfNotAlreadyThere (child);
    }

    for (int i = 0; i < component->getNumChildComponents(); ++i)
    {
        auto child = component->getChildComponent (i);
        if (child == nullptr)
            continue;

        for (const auto& foundComponent : findComponentsByType (child, typeName))
            result.addIfNotAlreadyThere (foundComponent);
    }

    return result;
}

juce::Array<juce::Component*> findComponentsByType (juce::StringRef typeName)
{
    juce::Array<juce::Component*> result;

    for (int i = 0; i < juce::Desktop::getInstance().getNumComponents(); ++i)
    {
        for (const auto& foundComponent : findComponentsByType (juce::Desktop::getInstance().getComponent (i), typeName))
            result.addIfNotAlreadyThere (foundComponent);
    }

    return result;
}

//=================================================================================================

juce::var makeComponentInfo (juce::Component* component, bool recursive)
{
    juce::DynamicObject::Ptr object = new juce::DynamicObject;

    if (component != nullptr)
    {
        object->setProperty ("id", component->getComponentID());
        object->setProperty ("name", component->getName());
        object->setProperty ("type", popsicle::Helpers::demangleClassName (typeid (*component).name()));
        object->setProperty ("visible", component->isVisible());
        object->setProperty ("showing", component->isShowing());
        object->setProperty ("enabled", component->isEnabled());
        object->setProperty ("window_handle", juce::String::formatted ("%p", component->getWindowHandle()));
        object->setProperty ("on_desktop", component->isOnDesktop());
        object->setProperty ("always_on_top", component->isAlwaysOnTop());
        object->setProperty ("currently_modal", component->isCurrentlyModal());
        object->setProperty ("opaque", component->isOpaque());
        object->setProperty ("alpha", component->getAlpha());
        object->setProperty ("accessible", component->isAccessible());
        object->setProperty ("transformed", component->isTransformed());
        object->setProperty ("transform", toVar (component->getTransform()));
        object->setProperty ("bounds", toVar (component->getBounds()));
        object->setProperty ("screen_bounds", toVar (component->getScreenBounds()));

        juce::DynamicObject::Ptr properties = new juce::DynamicObject;
        for (const auto& props : component->getProperties())
            properties->setProperty (props.name, props.value);
        object->setProperty ("properties", properties.get());

        object->setProperty ("title", component->getTitle());
        object->setProperty ("description", component->getDescription());
        object->setProperty ("help_text", component->getHelpText());

        object->setProperty ("num_children", component->getNumChildComponents());

        if (recursive)
        {
            juce::Array<juce::var> children;

            for (int i = 0; i < component->getNumChildComponents(); ++i)
                children.add (makeComponentInfo (component->getChildComponent(i), recursive));

            object->setProperty ("children", std::move (children));
        }
    }

    return object.get();
}

//=================================================================================================

juce::Image renderComponentToImage (juce::Component* component, bool withChildren)
{
    jassert (component != nullptr);
    jassert (component->getWidth() > 0);
    jassert (component->getHeight() > 0);

    auto image = juce::Image (juce::Image::ARGB, component->getWidth(), component->getHeight(), true);

    juce::Graphics graphics (image);

    if (withChildren)
        component->paintEntireComponent (graphics, true);
    else
        component->paint (graphics);

    return image;
}

//=================================================================================================

void clickComponent (juce::Component* component,
                     const juce::ModifierKeys& modifiersKeys,
                     std::function<void()> finishCallback,
                     juce::RelativeTime timeBetweenMouseDownAndUp)
{
    jassert (component != nullptr);
    jassert (juce::MessageManager::getInstance()->isThisTheMessageThread());

    auto mouseDownTime = juce::Time::getCurrentTime();
    auto mouseDownPos = component->getScreenBounds().getCentre().toFloat();

    juce::MouseEvent ev1
    {
        juce::Desktop::getInstance().getMainMouseSource(),
        mouseDownPos,
        modifiersKeys,
        1.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        component,
        component,
        mouseDownTime,
        mouseDownPos,
        mouseDownTime,
        1,
        false
    };

    component->mouseDown (ev1);

    auto elapsedMouseDownTime = juce::Time::getCurrentTime() - mouseDownTime;

    int clickTimeMilliseconds = juce::jmax(0, static_cast<int> (timeBetweenMouseDownAndUp.inMilliseconds() - elapsedMouseDownTime.inMilliseconds()));
    if (clickTimeMilliseconds > 0)
        juce::MessageManager::getInstance()->runDispatchLoopUntil(clickTimeMilliseconds);

    juce::MouseEvent ev2
    {
        juce::Desktop::getInstance().getMainMouseSource(),
        mouseDownPos,
        modifiersKeys,
        1.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        component,
        component,
        juce::Time::getCurrentTime(),
        mouseDownPos,
        mouseDownTime,
        1,
        false
    };

    component->mouseUp (ev2);

    if (finishCallback != nullptr)
        finishCallback();

    juce::MessageManager::getInstance()->runDispatchLoopUntil(1);
}

//=================================================================================================

juce::var invokeComponentCustomMethod (juce::Component* component,
                                       juce::StringRef methodName,
                                       const juce::Array<juce::var>& arguments,
                                       std::function<void(juce::StringRef)> errorCallback)
{
    if (component == nullptr)
        return juce::var();

    auto method = component->getProperties().getVarPointer (juce::String (methodName));
    if (method == nullptr)
        return errorCallback ? errorCallback("Method to invoke not found in object") : void(), juce::var();

    if (! method->isMethod())
        return errorCallback ? errorCallback("Method to invoke is not a method but something else") : void(), juce::var();

    juce::var::NativeFunctionArgs funcArgs (juce::var(), arguments.data(), arguments.size());
    return method->getNativeFunction() (funcArgs);
}

} // namespace straw::Helpers
