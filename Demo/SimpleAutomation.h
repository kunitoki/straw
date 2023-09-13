/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
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
