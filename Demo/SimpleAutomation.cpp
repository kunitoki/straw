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

#include "SimpleAutomation.h"

//=================================================================================================

juce::String CustomSlider::customMethod() const
{
    return "Called customMethod !";
}

PYBIND11_EMBEDDED_MODULE(custom, m)
{
    namespace py = pybind11;

    py::module_::import (popsicle::PythonModuleName);

    py::class_<CustomSlider, juce::Slider> (m, "CustomSlider")
        .def ("customMethod", &CustomSlider::customMethod);
}

//=================================================================================================

AutomationDemo::AutomationDemo()
{
    setOpaque (true);
    setComponentID ("straw::AutomationDemo");

    addAndMakeVisible (textButton);
    textButton.setComponentID ("straw::AutomationDemo::TextButton");
    textButton.setButtonText ("Click me !");
    textButton.addListener (this);
    textButton.getProperties().set ("example", 1337);

    addAndMakeVisible (slider);
    slider.setComponentID ("straw::AutomationDemo::Slider");
    slider.setSliderStyle (juce::Slider::LinearBar);

    setSize (300, 600);

    automationServer.registerDefaultEndpoints();

    auto weakThis = juce::Component::SafePointer<AutomationDemo>(this);
    automationServer.registerEndpoint ("/change_background_colour", [weakThis](straw::Request request)
    {
        auto colour = juce::Colour::fromString (request.data.getProperty ("colour", "FF00FF00").toString());

        juce::MessageManager::callAsync ([weakThis, colour, connection = std::move (request.connection)]
        {
            if (auto self = weakThis.getComponent())
            {
                self->setColour (juce::DocumentWindow::backgroundColourId, colour);
                self->repaint();

                straw::sendHttpResultResponse (true, 200, *connection);
            }
            else
            {
                straw::sendHttpErrorResponse("Unable to find the component!", 404, *connection);
            }
        });
    });

    automationServer.registerDefaultComponents();
    automationServer.registerCustomPythonModules ({ "custom" });
    automationServer.registerComponentType ("CustomSlider", &popsicle::ComponentType<CustomSlider>);

    auto result = automationServer.start (8001);
    if (result.failed())
        juce::Logger::writeToLog (result.getErrorMessage());
}

AutomationDemo::~AutomationDemo()
{
    automationServer.stop();
}

void AutomationDemo::paint (juce::Graphics& g)
{
    g.fillAll (findColour (juce::DocumentWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void AutomationDemo::resized()
{
    textButton.setBounds (10, 10, 200, 30);
    slider.setBounds (10, 50, 200, 30);

    dynamicComponent.setBounds (10, 100, 200, 30);
}

void AutomationDemo::parentHierarchyChanged()
{
    if (juce::Component* parent = getParentComponent())
        parent->setComponentID ("window");
}

void AutomationDemo::timerCallback()
{
    if (!isParentOf (&dynamicComponent))
    {
        dynamicComponent.setComponentID ("custom");
        addAndMakeVisible (dynamicComponent);

        stopTimer();
    }
}

void AutomationDemo::buttonClicked (juce::Button*)
{
    //juce::AlertWindow::showMessageBox (juce::AlertWindow::WarningIcon, "This modal component...", "...should disappear soon !");
    //startTimer (5000);

    slider.setVisible(! slider.isVisible());
}
