/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

namespace straw::Helpers {

//=================================================================================================

juce::String demangleClassName (juce::StringRef className);

//=================================================================================================

juce::Component* findComponentById (juce::Component* component, juce::StringRef id);

juce::Component* findComponentById (juce::StringRef id);

//=================================================================================================

juce::var makeComponentInfo (juce::Component* component, bool recursive = false);

//=================================================================================================

juce::Image renderComponentToImage (juce::Component* component, bool withChildren = false);

//=================================================================================================

void clickComponent (juce::StringRef componentID,
                     const juce::ModifierKeys& modifiersKeys,
                     std::function<void()> finishCallback,
                     juce::RelativeTime timeBetweenMouseDownAndUp = juce::RelativeTime::milliseconds(100));

} // namespace straw::Helpers
