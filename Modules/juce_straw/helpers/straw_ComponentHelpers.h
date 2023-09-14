/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

namespace straw::Helpers {

//=================================================================================================

/**
 * @brief Demangle a C++ class name.
 *
 * This function takes a StringRef containing a mangled class name and returns the demangled class name as a String.
 *
 * @param className The mangled class name to demangle.
 *
 * @return A String containing the demangled class name.
 */
juce::String demangleClassName (juce::StringRef className);

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
 * @brief Simulate a click event on a component with the specified ID.
 *
 * This function simulates a mouse click event on a component with the specified ID. It allows you to specify modifier keys, a callback to
 * execute after the click, and the time delay between mouse down and up events.
 *
 * @param componentID The ID of the component to click.
 * @param modifiersKeys The modifier keys to hold while clicking.
 * @param finishCallback A callback function to execute after the click.
 * @param timeBetweenMouseDownAndUp The time delay between mouse down and up events.
 */
void clickComponent (juce::StringRef componentID,
                     const juce::ModifierKeys& modifiersKeys,
                     std::function<void()> finishCallback,
                     juce::RelativeTime timeBetweenMouseDownAndUp = juce::RelativeTime::milliseconds(100));

} // namespace straw::Helpers
