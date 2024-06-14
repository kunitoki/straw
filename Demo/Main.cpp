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

#include "JuceHeader.h"
#include "SimpleAutomation.h"

//=================================================================================================

class Application : public juce::JUCEApplication
{
public:
    Application() = default;

    const juce::String getApplicationName() override
    {
        return "SimpleAutomationDemo";
    }

    const juce::String getApplicationVersion() override
    {
        return "1.0.0";
    }

    void initialise (const String&) override
    {
        mainWindow.reset (new MainWindow ("SimpleAutomationDemo", new AutomationDemo(), *this));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow (const juce::String& name, juce::Component* c, juce::JUCEApplication& a)
            : juce::DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel()
                                          .findColour (juce::ResizableWindow::backgroundColourId),
                                    juce::DocumentWindow::allButtons),
              app (a)
        {
            setComponentID ("straw::DemoMainWindow");
            setUsingNativeTitleBar (true);
            setContentOwned (c, true);

           #if JUCE_ANDROID || JUCE_IOS
            setFullScreen (true);
           #else
            setResizable (true, false);
            setResizeLimits (300, 250, 10000, 10000);
            centreWithSize (getWidth(), getHeight());
           #endif

            juce::MessageManager::callAsync([this]
            {
                juce::Process::makeForegroundProcess();

                setVisible (true);
                toFront (true);
            });
        }

        void closeButtonPressed() override
        {
            app.systemRequestedQuit();
        }

    private:
        juce::JUCEApplication& app;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;
};

//=================================================================================================

START_JUCE_APPLICATION (Application)
