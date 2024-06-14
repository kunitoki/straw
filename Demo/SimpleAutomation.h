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

#include "JuceHeader.h"

//=================================================================================================

class CustomSlider : public juce::Slider
{
public:
    using juce::Slider::Slider;

    juce::String customMethod() const;
};

//=================================================================================================

class AutomationDemo
    : public juce::Component
    , public juce::Timer
    , public juce::Button::Listener
{
public:
    AutomationDemo();
    ~AutomationDemo() override;

    // juce::Component
    void paint (juce::Graphics& g) override;
    void resized() override;
    void parentHierarchyChanged() override;

    // juce::Timer
    void timerCallback() override;

    // juce::Button::Listener
    void buttonClicked (juce::Button*) override;

private:
    juce::TextButton textButton;
    juce::TextButton dynamicComponent;
    CustomSlider slider;

    straw::AutomationServer automationServer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationDemo)
};
