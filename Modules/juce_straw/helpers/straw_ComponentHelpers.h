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

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

namespace straw::Helpers {

//=================================================================================================

/**
 * @brief Find a component by its ID within a given component and its children.
 *
 * This function searches for a component with the specified ID within the hierarchy of the provided component and its child components.
 * It returns a pointer to the first component found with a matching ID or nullptr if no matching component is found.
 *
 * @param component The root component to start the search from.
 * @param id The ID to search for.
 *
 * @return A pointer to the found component, or nullptr if not found.
 */
juce::Component* findComponentById (juce::Component* component, juce::StringRef id);

/**
 * @brief Find a top-level component by its ID within the entire component hierarchy.
 *
 * This function searches for a component with the specified ID within the entire component hierarchy of the application. It returns a pointer
 * to the first component found with a matching ID or nullptr if no matching component is found.
 *
 * @param id The ID to search for.
 *
 * @return A pointer to the found component, or nullptr if not found.
 */
juce::Component* findComponentById (juce::StringRef id);

//=================================================================================================

juce::Array<juce::Component*> findComponentsByType (juce::Component* component, juce::StringRef typeName);

juce::Array<juce::Component*> findComponentsByType (juce::StringRef typeName);

//=================================================================================================

/**
 * @brief Generate information about a component and its hierarchy.
 *
 * This function creates a JSON-like juce::var object containing information about the provided component and its child components. The
 * `recursive` parameter determines whether to include information about child components recursively.
 *
 * @param component The root component to generate information for.
 * @param recursive If true, include information about child components recursively.
 *
 * @return A juce::var object containing the component information.
 */
juce::var makeComponentInfo (juce::Component* component, bool recursive = false);

//=================================================================================================

/**
 * @brief Render a component and its children to an image.
 *
 * This function renders the provided component and its child components to an image. The `withChildren` parameter determines
 * whether to include child components in the rendered image.
 *
 * @param component The root component to render.
 * @param withChildren If true, include child components in the rendered image.
 *
 * @return An image representing the rendered component.
 */
juce::Image renderComponentToImage (juce::Component* component, bool withChildren = false);

//=================================================================================================

/**
 * @brief Simulate a click event on a component.
 *
 * This function simulates a mouse click event on a component. It allows you to specify modifier keys, a callback to
 * execute after the click, and the time delay between mouse down and up events.
 *
 * @param component The component to click.
 * @param modifiersKeys The modifier keys to hold while clicking.
 * @param finishCallback A callback function to execute after the click.
 * @param timeBetweenMouseDownAndUp The time delay between mouse down and up events.
 */
void clickComponent (juce::Component* component,
                     const juce::ModifierKeys& modifiersKeys,
                     std::function<void()> finishCallback,
                     juce::RelativeTime timeBetweenMouseDownAndUp = juce::RelativeTime::milliseconds(100));

//=================================================================================================

juce::var invokeComponentCustomMethod (juce::Component* component,
                                       juce::StringRef methodName,
                                       const juce::Array<juce::var>& arguments,
                                       std::function<void(juce::StringRef)> errorCallback = nullptr);

} // namespace straw::Helpers
