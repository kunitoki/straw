/**
 * straw 4 the juce - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
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
