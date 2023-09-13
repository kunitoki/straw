/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "straw_ComponentHelpers.h"

#include "../values/straw_VariantConverter.h"

#if __clang__ || GNUC
#include <cxxabi.h>
#else
#include <windows.h>
#include <dbghelp.h>
#pragma comment (lib, "dbghelp.lib")
extern char* __unDName (char*, const char*, int, void*, void*, int);
#endif

namespace straw::Helpers {

//=================================================================================================

juce::String demangleClassName (juce::StringRef className)
{
    juce::String name = className;

#if __clang__ || GNUC
    int status = -1;
    char* demangledName = abi::__cxa_demangle (name.toUTF8(), nullptr, nullptr, &status);
    name = demangledName;
    std::free (demangledName);
#else
    char demangledName [1024] = { 0 };
    __unDName (demangledName, name.toUTF8() + 1, juce::numElementsInArray (demangledName), malloc, free, 0x2800);
    name = demangledName;
#endif

    return name;
}

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

juce::var makeComponentInfo (juce::Component* component, bool recursive)
{
    juce::DynamicObject::Ptr object = new juce::DynamicObject;

    if (component != nullptr)
    {
        object->setProperty ("id", component->getComponentID());
        object->setProperty ("name", component->getName());
        object->setProperty ("type", demangleClassName (typeid (*component).name()));
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

void clickComponent (juce::StringRef componentID,
                     const juce::ModifierKeys& modifiersKeys,
                     std::function<void()> finishCallback,
                     juce::RelativeTime timeBetweenMouseDownAndUp)
{
    jassert (juce::MessageManager::getInstance()->isThisTheMessageThread());
    
    auto mouseDownTime = juce::Time::getCurrentTime();
    
    juce::Component* component = Helpers::findComponentById (componentID);
    if (component == nullptr)
    {
        finishCallback();
        return;
    }
    
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

} // namespace straw::Helpers
