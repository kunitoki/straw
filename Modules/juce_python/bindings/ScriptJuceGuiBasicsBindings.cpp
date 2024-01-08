/**
 * juce python - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "ScriptJuceGuiBasicsBindings.h"

//#include "../../values/straw_VariantConverter.h"

#include "../pybind11/operators.h"

#include <functional>
#include <string_view>
#include <typeinfo>
#include <tuple>

//=================================================================================================

namespace PYBIND11_NAMESPACE {

template <>
struct polymorphic_type_hook<juce::Component>
{
    static const void* get (const juce::Component* src, const std::type_info*& type)
    {
        if (src == nullptr)
            return src;

        auto& map = jucepy::Bindings::getComponentTypeMap();
        auto demangledName = jucepy::Helpers::demangleClassName (typeid(*src).name());

        auto it = map.typeMap.find (demangledName);
        if (it != map.typeMap.end())
            return it->second (src, type);

        return src;
    }
};

} // namespace PYBIND11_NAMESPACE

//=================================================================================================

namespace juce {

#if JUCE_MAC
 extern void initialiseNSApplication();
#endif

extern const char* const* juce_argv;
extern int juce_argc;

} // namespace juce

//=================================================================================================

namespace jucepy::Bindings {

void registerJuceGuiBasicsBindings (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

#if 1 // ! JUCE_PYTHON_EMBEDDED_INTERPRETER

    // ============================================================================================ START_JUCE_APPLICATION

    m.def ("START_JUCE_APPLICATION", [](py::handle applicationType)
    {
        if (! applicationType)
            throw py::value_error("Argument must be a JUCEApplication subclass");
    
        JUCEApplicationBase::createInstance = +[]() -> JUCEApplicationBase* { return nullptr; };

        initialiseJuce_GUI();
        const ScopeGuard shutdownJuceAtExit { [] { shutdownJuce_GUI(); } };

#if JUCE_MAC
        initialiseNSApplication();
#elif JUCE_WINDOWS
		Process::setCurrentModuleInstanceHandle();
#endif

        JUCEApplicationBase* application = nullptr;

        auto sys = py::module_::import ("sys");
        auto systemExit = [sys, &application]
        {
            const int returnValue = application != nullptr ? application->shutdownApp() : 255;
        
            sys.attr ("exit") (returnValue);
        };

        StringArray arguments;
        for (auto arg : sys.attr ("argv"))
            arguments.add (arg.cast<String>());

        Array<const char*> argv;
        for (const auto& arg : arguments)
            argv.add (arg.toRawUTF8());

        juce_argv = argv.getRawDataPointer();
        juce_argc = argv.size();

        auto pyApplication = applicationType(); // TODO - error checking (python)

        application = pyApplication.cast<JUCEApplication*>();
        if (application == nullptr)
        {
            systemExit();
            return;
        }

        try
        {
            if (! application->initialiseApp()) // TODO - error checking (python)
            {
                systemExit();
                return;
            }

            bool isErrorSignalInFlight = false;

            JUCE_TRY
            {
                do
                {
                    {
                        py::gil_scoped_release release;

                        if (MessageManager::getInstance()->hasStopMessageBeenSent())
                            break;

                        MessageManager::getInstance()->runDispatchLoopUntil(200);
                    }
                    
                    isErrorSignalInFlight = PyErr_CheckSignals() != 0;
                    if (isErrorSignalInFlight)
                        break;
                }
                while (true);
            }
            JUCE_CATCH_EXCEPTION
            
            if (isErrorSignalInFlight)
                throw py::error_already_set();
        }
        catch (const py::error_already_set& e)
        {
            py::print (e.what());
        }
        catch (...)
        {
            py::print ("unhandled runtime error");
        }

        systemExit();
    });

#endif

    // ============================================================================================ juce::JUCEApplication

    struct PyJUCEApplication : public JUCEApplication
    {
        const String getApplicationName() override
        {
            PYBIND11_OVERRIDE_PURE (const String, JUCEApplication, getApplicationName);
        }

        const String getApplicationVersion() override
        {
            PYBIND11_OVERRIDE_PURE (const String, JUCEApplication, getApplicationVersion);
        }

        bool moreThanOneInstanceAllowed() override
        {
            PYBIND11_OVERRIDE (bool, JUCEApplication, moreThanOneInstanceAllowed);
        }

        void initialise (const String& commandLineParameters) override
        {
            PYBIND11_OVERRIDE_PURE (void, JUCEApplication, initialise, commandLineParameters);
        }

        void shutdown() override
        {
            PYBIND11_OVERRIDE_PURE (void, JUCEApplication, shutdown);
        }

        void anotherInstanceStarted (const String& commandLine) override
        {
            PYBIND11_OVERRIDE (void, JUCEApplication, anotherInstanceStarted, commandLine);
        }

        void systemRequestedQuit() override
        {
            PYBIND11_OVERRIDE (void, JUCEApplication, systemRequestedQuit);
        }

        void suspended() override
        {
            PYBIND11_OVERRIDE (void, JUCEApplication, suspended);
        }

        void resumed() override
        {
            PYBIND11_OVERRIDE (void, JUCEApplication, resumed);
        }

        void unhandledException (const std::exception* ex, const String& sourceFilename, int lineNumber) override
        {
            //PYBIND11_OVERRIDE (void, JUCEApplication, unhandledException, ex, sourceFilename, lineNumber);

            {
                py::gil_scoped_acquire gil;

                if (py::function override_ = py::get_override (static_cast<JUCEApplication*> (this), "unhandledException"); override_)
                {
                    //String code;
                    //code << "RuntimeError(\"\"\"" << (ex != nullptr ? ex->what() : "unhandled exception") << "\"\"\")";
                    //auto pyex = py::eval (code.toRawUTF8());

                    //auto pyex = exceptionClass (ex != nullptr ? ex->what() : "unhandled exception");

                    auto pyex = py::none();

                    override_ (pyex, sourceFilename, lineNumber);
                    
                    return;
                }
            }

            std::terminate();
        }

        void memoryWarningReceived() override
        {
            PYBIND11_OVERRIDE (void, JUCEApplication, memoryWarningReceived);
        }

        bool backButtonPressed() override
        {
            PYBIND11_OVERRIDE (bool, JUCEApplication, backButtonPressed);
        }
    };

    py::class_<JUCEApplication, PyJUCEApplication> (m, "JUCEApplication")
        .def (py::init<>())
        .def_static ("getInstance", &JUCEApplication::getInstance, py::return_value_policy::reference)
        .def ("getApplicationName", &JUCEApplication::getApplicationName)
        .def ("getApplicationVersion", &JUCEApplication::getApplicationVersion)
        .def ("moreThanOneInstanceAllowed", &JUCEApplication::moreThanOneInstanceAllowed)
        .def ("initialise", &JUCEApplication::initialise)
        .def ("shutdown", &JUCEApplication::shutdown)
        .def ("anotherInstanceStarted", &JUCEApplication::anotherInstanceStarted)
        .def ("systemRequestedQuit", &JUCEApplication::systemRequestedQuit)
        .def ("suspended", &JUCEApplication::suspended)
        .def ("resumed", &JUCEApplication::resumed)
        .def ("unhandledException", &JUCEApplication::unhandledException)
        .def ("memoryWarningReceived", &JUCEApplication::memoryWarningReceived)
        .def_static ("quit", &JUCEApplication::quit)
        .def_static ("getCommandLineParameterArray", &JUCEApplication::getCommandLineParameterArray)
        .def_static ("getCommandLineParameters", &JUCEApplication::getCommandLineParameters)
        .def ("setApplicationReturnValue", &JUCEApplication::setApplicationReturnValue)
        .def ("getApplicationReturnValue", &JUCEApplication::getApplicationReturnValue)
        .def_static ("isStandaloneApp", &JUCEApplication::isStandaloneApp)
        .def ("isInitialising", &JUCEApplication::isInitialising)
    ;

    // ============================================================================================ juce::Component

    class PyComponent : public Component
    {
        using Component::Component;
    
        void setName (const String& newName) override
        {
            PYBIND11_OVERRIDE (void, Component, setName, newName);
        }
        
        void setVisible (bool shouldBeVisible) override
        {
            PYBIND11_OVERRIDE (void, Component, setVisible, shouldBeVisible);
        }

        void visibilityChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, visibilityChanged);
        }

        void userTriedToCloseWindow() override
        {
            PYBIND11_OVERRIDE (void, Component, userTriedToCloseWindow);
        }

        void minimisationStateChanged(bool isNowMinimised) override
        {
            PYBIND11_OVERRIDE (void, Component, minimisationStateChanged, isNowMinimised);
        }

        float getDesktopScaleFactor() const override
        {
            PYBIND11_OVERRIDE (float, Component, getDesktopScaleFactor);
        }
        
        void parentHierarchyChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, parentHierarchyChanged);
        }

        void childrenChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, childrenChanged);
        }
        
        bool hitTest (int x, int y) override
        {
            PYBIND11_OVERRIDE (bool, Component, hitTest, x, y);
        }
        
        void lookAndFeelChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, lookAndFeelChanged);
        }
        
        void enablementChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, enablementChanged);
        }

        void alphaChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, alphaChanged);
        }
        
        void paint (Graphics& g) override
        {
            {
                py::gil_scoped_acquire gil;

                if (py::function override_ = py::get_override (static_cast<const Component*> (this), "paint"); override_)
                {
                    override_ (std::addressof (g));
                    return;
                }
            }

            return Component::paint (g);
        }

        void paintOverChildren (Graphics& g) override
        {
            {
                py::gil_scoped_acquire gil;

                if (py::function override_ = py::get_override (static_cast<const Component*> (this), "paintOverChildren"); override_)
                {
                    override_ (std::addressof (g));
                    return;
                }
            }

            return Component::paintOverChildren (g);
        }
    };

    py::class_<Component, PyComponent> (m, "Component")
        .def (py::init<>())
        .def (py::init<const String&>())

        .def ("setName", &Component::setName)
        .def ("getName", &Component::getName)
        .def ("getComponentID", &Component::getComponentID)

        .def ("setVisible", &Component::setVisible)
        .def ("isVisible", &Component::isVisible)
        .def ("visibilityChanged", &Component::visibilityChanged)
        .def ("isShowing", &Component::isShowing)

    //.def ("addToDesktop", &Component::addToDesktop)
        .def ("removeFromDesktop", &Component::removeFromDesktop)
        .def ("isOnDesktop", &Component::isOnDesktop)
        .def ("getDesktopScaleFactor", &Component::getDesktopScaleFactor)
    //.def ("getPeer", &Component::getPeer)
        .def ("userTriedToCloseWindow", &Component::userTriedToCloseWindow)
        .def ("minimisationStateChanged", &Component::minimisationStateChanged)
        .def ("getDesktopScaleFactor", &Component::getDesktopScaleFactor)

        .def ("toFront", &Component::toFront)
        .def ("toBack", &Component::toBack)
        .def ("toBehind", &Component::toBehind)
        .def ("setAlwaysOnTop", &Component::setAlwaysOnTop)
        .def ("isAlwaysOnTop", &Component::isAlwaysOnTop)
        
        .def ("getX", &Component::getX)
        .def ("getY", &Component::getY)
        .def ("getWidth", &Component::getWidth)
        .def ("getHeight", &Component::getHeight)
        .def ("getRight", &Component::getRight)
        .def ("getBottom", &Component::getBottom)
        .def ("getPosition", &Component::getPosition)
        .def ("getBounds", &Component::getBounds)
        .def ("getLocalBounds", &Component::getLocalBounds)
        .def ("getBoundsInParent", &Component::getBoundsInParent)
        .def ("getScreenX", &Component::getScreenX)
        .def ("getScreenY", &Component::getScreenY)
        .def ("getScreenPosition", &Component::getScreenPosition)
        .def ("getScreenBounds", &Component::getScreenBounds)

        .def ("setTopLeftPosition", py::overload_cast<int, int>(&Component::setTopLeftPosition))
        .def ("setTopLeftPosition", py::overload_cast<Point<int>>(&Component::setTopLeftPosition))
        .def ("setTopRightPosition", &Component::setTopRightPosition)
        .def ("setSize", &Component::setSize)
        .def ("setBounds", py::overload_cast<int, int, int, int>(&Component::setBounds))
        .def ("setBounds", py::overload_cast<Rectangle<int>>(&Component::setBounds))
        .def ("setBoundsRelative", py::overload_cast<float, float, float, float>(&Component::setBoundsRelative))
        .def ("setBoundsRelative", py::overload_cast<Rectangle<float>>(&Component::setBoundsRelative))
        .def ("setBoundsInset", &Component::setBoundsInset)
        .def ("setBoundsToFit", &Component::setBoundsToFit)
        .def ("setCentrePosition", py::overload_cast<int, int>(&Component::setCentrePosition))
        .def ("setCentrePosition", py::overload_cast<Point<int>>(&Component::setCentrePosition))
        .def ("setCentreRelative", &Component::setCentreRelative)
        .def ("centreWithSize", &Component::centreWithSize)

        .def ("setTransform", &Component::setTransform)
        .def ("getTransform", &Component::getTransform)
        .def ("isTransformed", &Component::isTransformed)
        .def_static ("getApproximateScaleFactorForComponent", &Component::getApproximateScaleFactorForComponent)
        .def ("proportionOfWidth", &Component::proportionOfWidth)
        .def ("proportionOfHeight", &Component::proportionOfHeight)
        .def ("getParentWidth", &Component::getParentWidth)
        .def ("getParentHeight", &Component::getParentHeight)
        .def ("getParentMonitorArea", &Component::getParentMonitorArea)

        .def ("getNumChildComponents", &Component::getNumChildComponents)
        .def ("getChildComponent", &Component::getChildComponent, py::return_value_policy::reference)
        .def ("getIndexOfChildComponent", &Component::getIndexOfChildComponent)
        .def ("findChildWithID", &Component::findChildWithID)
        .def ("addChildComponent", py::overload_cast<Component*, int>(&Component::addChildComponent))
        .def ("addAndMakeVisible", py::overload_cast<Component*, int>(&Component::addAndMakeVisible))
        .def ("addChildAndSetID", &Component::addChildAndSetID)
        .def ("removeChildComponent", py::overload_cast<Component*>(&Component::removeChildComponent))
        .def ("removeChildComponent", py::overload_cast<int>(&Component::removeChildComponent), py::return_value_policy::reference)
        .def ("removeAllChildren", &Component::removeAllChildren)
        .def ("deleteAllChildren", &Component::deleteAllChildren)
        .def ("getParentComponent", &Component::getParentComponent, py::return_value_policy::reference)
        .def ("getTopLevelComponent", &Component::getTopLevelComponent, py::return_value_policy::reference)
        .def ("isParentOf", &Component::isParentOf)

        .def ("parentHierarchyChanged", &Component::parentHierarchyChanged)
        .def ("childrenChanged", &Component::childrenChanged)

        .def ("hitTest", &Component::hitTest)
        .def ("setInterceptsMouseClicks", &Component::setInterceptsMouseClicks)
    //.def ("getInterceptsMouseClicks", &Component::getInterceptsMouseClicks)
        .def ("contains", py::overload_cast<Point<float>>(&Component::contains))
        .def ("reallyContains", py::overload_cast<Point<float>, bool>(&Component::reallyContains))
        .def ("getComponentAt", py::overload_cast<int, int>(&Component::getComponentAt))
        .def ("getComponentAt", py::overload_cast<Point<float>>(&Component::getComponentAt))

        .def ("repaint", py::overload_cast<>(&Component::repaint))
        .def ("repaint", py::overload_cast<int, int, int, int>(&Component::repaint))
        .def ("repaint", py::overload_cast<Rectangle<int>>(&Component::repaint))
        .def ("setBufferedToImage", &Component::setBufferedToImage)
        .def ("createComponentSnapshot", &Component::createComponentSnapshot)
        .def ("paintEntireComponent", &Component::paintEntireComponent)
        .def ("setPaintingIsUnclipped", &Component::setPaintingIsUnclipped)
        .def ("isPaintingUnclipped", &Component::isPaintingUnclipped)

        //.def ("setComponentEffect", &Component::setComponentEffect)
        //.def ("getComponentEffect", &Component::getComponentEffect)

        //.def ("getLookAndFeel", &Component::getLookAndFeel)
        //.def ("setLookAndFeel", &Component::setLookAndFeel)
        .def ("lookAndFeelChanged", &Component::lookAndFeelChanged)
        .def ("sendLookAndFeelChange", &Component::sendLookAndFeelChange)

        .def ("setOpaque", &Component::setOpaque)
        .def ("isOpaque", &Component::isOpaque)

        .def ("setBroughtToFrontOnMouseClick", &Component::setBroughtToFrontOnMouseClick)
        .def ("isBroughtToFrontOnMouseClick", &Component::isBroughtToFrontOnMouseClick)

        .def ("setExplicitFocusOrder", &Component::setExplicitFocusOrder)
        .def ("getExplicitFocusOrder", &Component::getExplicitFocusOrder)
    //.def ("setFocusContainerType", &Component::setFocusContainerType)
        .def ("isFocusContainer", &Component::isFocusContainer)
        .def ("isKeyboardFocusContainer", &Component::isKeyboardFocusContainer)
        .def ("findFocusContainer", &Component::findFocusContainer, py::return_value_policy::reference)
        .def ("findKeyboardFocusContainer", &Component::findKeyboardFocusContainer, py::return_value_policy::reference)

        .def ("setWantsKeyboardFocus", &Component::setWantsKeyboardFocus)
        .def ("getWantsKeyboardFocus", &Component::getWantsKeyboardFocus)
        .def ("setMouseClickGrabsKeyboardFocus", &Component::setMouseClickGrabsKeyboardFocus)
        .def ("getMouseClickGrabsKeyboardFocus", &Component::getMouseClickGrabsKeyboardFocus)
        .def ("grabKeyboardFocus", &Component::grabKeyboardFocus)
        .def ("giveAwayKeyboardFocus", &Component::giveAwayKeyboardFocus)
        .def ("hasKeyboardFocus", &Component::hasKeyboardFocus)
        .def ("moveKeyboardFocusToSibling", &Component::moveKeyboardFocusToSibling)
        .def_static ("getCurrentlyFocusedComponent", &Component::getCurrentlyFocusedComponent, py::return_value_policy::reference)
        .def_static ("unfocusAllComponents", &Component::unfocusAllComponents)

    //.def ("createFocusTraverser", &Component::createFocusTraverser)
    //.def ("createKeyboardFocusTraverser", &Component::createKeyboardFocusTraverser)
        .def ("setHasFocusOutline", &Component::setHasFocusOutline)
        .def ("hasFocusOutline", &Component::hasFocusOutline)

        .def ("isEnabled", &Component::isEnabled)
        .def ("setEnabled", &Component::setEnabled)
        .def ("enablementChanged", &Component::enablementChanged)

        .def ("getAlpha", &Component::getAlpha)
        .def ("setAlpha", &Component::setAlpha)
        .def ("alphaChanged", &Component::alphaChanged)

    //.def ("setMouseCursor", &Component::setMouseCursor)
    //.def ("getMouseCursor", &Component::getMouseCursor)
        .def ("updateMouseCursor", &Component::updateMouseCursor)

        .def ("paint", &Component::paint)
        .def ("paintOverChildren", &Component::paintOverChildren)

        .def ("getMouseXYRelative", &Component::getMouseXYRelative)
        .def ("isCurrentlyBlockedByAnotherModalComponent", &Component::isCurrentlyBlockedByAnotherModalComponent)
        .def ("getProperties", py::overload_cast<>(&Component::getProperties))
        //.def ("getPositioner", &Component::getPositioner)
        //.def ("getCachedComponentImage", &Component::getCachedComponentImage)
        .def ("getViewportIgnoreDragFlag", &Component::getViewportIgnoreDragFlag)
        .def ("getTitle", &Component::getTitle)
        .def ("getDescription", &Component::getDescription)
        .def ("getHelpText", &Component::getHelpText)
        .def ("isAccessible", &Component::isAccessible)
        //.def ("getAccessibilityHandler", &Component::getAccessibilityHandler)
#if JUCE_MODAL_LOOPS_PERMITTED
        .def ("runModalLoop", &Component::runModalLoop)
#endif

        .def ("getChildren", [](const juce::Component& self)
        {
            py::list list;
            for (const auto& comp : self.getChildren())
                list.append (comp);
            return list;
        })

        .def ("typeName", [](const juce::Component* self)
        {
            return Helpers::demangleClassName (typeid(*self).name());
        })
    ;

    // ============================================================================================ juce::Button

    py::class_<Button, Component> (m, "Button")
        .def ("getButtonText", &Button::getButtonText)
        .def ("isDown", &Button::isDown)
        .def ("isOver", &Button::isOver)
        .def ("isToggleable", &Button::isToggleable)
        .def ("getToggleState", &Button::getToggleState)
        //.def ("getToggleStateValue", &Button::getToggleStateValue)
        .def ("getClickingTogglesState", &Button::getClickingTogglesState)
        .def ("getRadioGroupId", &Button::getRadioGroupId)
        .def ("triggerClick", &Button::triggerClick)
        .def ("getCommandID", &Button::getCommandID)
        .def ("getTriggeredOnMouseDown", &Button::getTriggeredOnMouseDown)
        .def ("getMillisecondsSinceButtonDown", &Button::getMillisecondsSinceButtonDown)
        .def ("getConnectedEdgeFlags", &Button::getConnectedEdgeFlags)
        .def ("isConnectedOnLeft", &Button::isConnectedOnLeft)
        .def ("isConnectedOnRight", &Button::isConnectedOnRight)
        .def ("isConnectedOnTop", &Button::isConnectedOnTop)
        .def ("isConnectedOnBottom", &Button::isConnectedOnBottom)
        //.def ("getState", &Button::getState)
    ;

    py::class_<ArrowButton, Button> (m, "ArrowButton");

    py::class_<DrawableButton, Button> (m, "DrawableButton")
        //.def ("getStyle", &DrawableButton::getStyle)
        .def ("getEdgeIndent", &DrawableButton::getEdgeIndent)
        //.def ("getCurrentImage", &DrawableButton::getCurrentImage, py::return_value_policy::reference)
        //.def ("getNormalImage", &DrawableButton::getNormalImage, py::return_value_policy::reference)
        //.def ("getOverImage", &DrawableButton::getOverImage, py::return_value_policy::reference)
        //.def ("getDownImage", &DrawableButton::getDownImage, py::return_value_policy::reference)
        .def ("getImageBounds", &DrawableButton::getImageBounds)
    ;

    py::class_<HyperlinkButton, Button> (m, "HyperlinkButton")
        //.def ("getURL", &HyperlinkButton::getURL)
        //.def ("getJustificationType", &HyperlinkButton::getJustificationType)
    ;

    py::class_<ImageButton, Button> (m, "ImageButton")
        .def ("getNormalImage", &ImageButton::getNormalImage)
        .def ("getOverImage", &ImageButton::getOverImage)
        .def ("getDownImage", &ImageButton::getDownImage)
    ;

    py::class_<ShapeButton, Button> (m, "ShapeButton");

    py::class_<TextButton, Button> (m, "TextButton")
        .def ("getBestWidthForHeight", &TextButton::getBestWidthForHeight)
    ;

    py::class_<ToggleButton, Button> (m, "ToggleButton");

    // ============================================================================================ juce::Slider

    py::class_<Slider, Component> (m, "Slider")
        .def ("getMouseDragSensitivity", &Slider::getMouseDragSensitivity)
        .def ("getVelocityBasedMode", &Slider::getVelocityBasedMode)
        .def ("getVelocitySensitivity", &Slider::getVelocitySensitivity)
        .def ("getVelocityThreshold", &Slider::getVelocityThreshold)
        .def ("getVelocityOffset", &Slider::getVelocityOffset)
        .def ("getVelocityModeIsSwappable", &Slider::getVelocityModeIsSwappable)
        .def ("getSkewFactor", &Slider::getSkewFactor)
        .def ("isSymmetricSkew", &Slider::isSymmetricSkew)
        .def ("getTextBoxWidth", &Slider::getTextBoxWidth)
        .def ("getTextBoxHeight", &Slider::getTextBoxHeight)
        .def ("isTextBoxEditable", &Slider::isTextBoxEditable)
        .def ("showTextBox", &Slider::showTextBox)
        .def ("hideTextBox", &Slider::hideTextBox)
        .def ("getValue", &Slider::getValue)
        .def ("getMaximum", &Slider::getMaximum)
        .def ("getMinimum", &Slider::getMinimum)
        .def ("getInterval", &Slider::getInterval)
        .def ("getMinValue", &Slider::getMinValue)
        .def ("getMaxValue", &Slider::getMaxValue)
        .def ("getDoubleClickReturnValue", &Slider::getDoubleClickReturnValue)
        .def ("isDoubleClickReturnEnabled", &Slider::isDoubleClickReturnEnabled)
        .def ("getSliderSnapsToMousePosition", &Slider::getSliderSnapsToMousePosition)
        .def ("getCurrentPopupDisplay", &Slider::getCurrentPopupDisplay)
        .def ("isScrollWheelEnabled", &Slider::isScrollWheelEnabled)
        .def ("getThumbBeingDragged", &Slider::getThumbBeingDragged)
        .def ("getValueFromText", &Slider::getValueFromText)
        .def ("getTextFromValue", &Slider::getTextFromValue)
        .def ("getTextValueSuffix", &Slider::getTextValueSuffix)
        .def ("getNumDecimalPlacesToDisplay", &Slider::getNumDecimalPlacesToDisplay)
        .def ("proportionOfLengthToValue", &Slider::proportionOfLengthToValue)
        .def ("valueToProportionOfLength", &Slider::valueToProportionOfLength)
        .def ("getPositionOfValue", &Slider::getPositionOfValue)
        .def ("updateText", &Slider::updateText)
        .def ("isHorizontal", &Slider::isHorizontal)
        .def ("isVertical", &Slider::isVertical)
        .def ("isRotary", &Slider::isRotary)
        .def ("isBar", &Slider::isBar)
        .def ("isTwoValue", &Slider::isTwoValue)
        .def ("isThreeValue", &Slider::isThreeValue)
    ;

    // ============================================================================================ juce::TopLevelWindow

    py::class_<TopLevelWindow, Component> classTopLevelWindow (m, "TopLevelWindow");
    classTopLevelWindow
        .def (py::init<const String&, bool>())
        .def ("isActiveWindow", &TopLevelWindow::isActiveWindow)
        .def ("centreAroundComponent", &TopLevelWindow::centreAroundComponent)
        .def ("setDropShadowEnabled", &TopLevelWindow::setDropShadowEnabled)
        .def ("isDropShadowEnabled", &TopLevelWindow::isDropShadowEnabled)
        .def ("setUsingNativeTitleBar", &TopLevelWindow::setUsingNativeTitleBar)
        .def ("isUsingNativeTitleBar", &TopLevelWindow::isUsingNativeTitleBar)
        .def_static ("getNumTopLevelWindows", &TopLevelWindow::getNumTopLevelWindows)
        .def_static ("getTopLevelWindow", &TopLevelWindow::getTopLevelWindow, py::return_value_policy::reference)
        .def_static ("getActiveTopLevelWindow", &TopLevelWindow::getActiveTopLevelWindow, py::return_value_policy::reference)
        .def ("addToDesktop", py::overload_cast<>(&TopLevelWindow::addToDesktop))
    ;

    // ============================================================================================ juce::ResizableWindow

    py::class_<ResizableWindow, TopLevelWindow> classResizableWindow (m, "ResizableWindow");
    classResizableWindow
        .def (py::init<const String&, bool>())
        .def (py::init<const String&, Colour, bool>())
        .def ("getBackgroundColour", &ResizableWindow::getBackgroundColour)
        .def ("setBackgroundColour", &ResizableWindow::setBackgroundColour)
        .def ("setResizable", &ResizableWindow::setResizable)
        .def ("isResizable", &ResizableWindow::isResizable)
        .def ("setResizeLimits", &ResizableWindow::setResizeLimits)
        .def ("setDraggable", &ResizableWindow::setDraggable)
        .def ("isDraggable", &ResizableWindow::isDraggable)
    //.def ("getConstrainer", &ResizableWindow::getConstrainer)
    //.def ("setConstrainer", &ResizableWindow::setConstrainer)
        .def ("setBoundsConstrained", &ResizableWindow::setBoundsConstrained)
        .def ("isFullScreen", &ResizableWindow::isFullScreen)
        .def ("setFullScreen", &ResizableWindow::setFullScreen)
        .def ("isMinimised", &ResizableWindow::isMinimised)
        .def ("setMinimised", &ResizableWindow::setMinimised)
        .def ("isKioskMode", &ResizableWindow::isKioskMode)
        .def ("getWindowStateAsString", &ResizableWindow::getWindowStateAsString)
        .def ("restoreWindowStateFromString", &ResizableWindow::restoreWindowStateFromString)
        .def ("getContentComponent", &ResizableWindow::getContentComponent, py::return_value_policy::reference)
        .def ("setContentOwned", &ResizableWindow::setContentOwned)
        .def ("setContentNonOwned", &ResizableWindow::setContentNonOwned)
        .def ("clearContentComponent", &ResizableWindow::clearContentComponent)
        .def ("setContentComponentSize", &ResizableWindow::setContentComponentSize)
    //.def ("getBorderThickness", &ResizableWindow::getBorderThickness)
    //.def ("getContentComponentBorder", &ResizableWindow::getContentComponentBorder)
    ;

    // ============================================================================================ juce::DocumentWindow

    class PyDocumentWindow : public DocumentWindow
    {
        using DocumentWindow::DocumentWindow;
    
        void closeButtonPressed() override
        {
            PYBIND11_OVERRIDE(void, DocumentWindow, closeButtonPressed);
        }

        void minimiseButtonPressed() override
        {
            PYBIND11_OVERRIDE(void, DocumentWindow, minimiseButtonPressed);
        }

        void maximiseButtonPressed() override
        {
            PYBIND11_OVERRIDE(void, DocumentWindow, maximiseButtonPressed);
        }
    };

    py::class_<DocumentWindow, ResizableWindow, PyDocumentWindow> classDocumentWindow (m, "DocumentWindow");
    classDocumentWindow
        .def (py::init<const String&, Colour, int>())
        .def (py::init<const String&, Colour, int, bool>())
        .def ("setName", &DocumentWindow::setName)
        .def ("setIcon", &DocumentWindow::setIcon)
        .def ("setTitleBarHeight", &DocumentWindow::setTitleBarHeight)
        .def ("getTitleBarHeight", &DocumentWindow::getTitleBarHeight)
        .def ("setTitleBarButtonsRequired", &DocumentWindow::setTitleBarButtonsRequired)
        .def ("setTitleBarTextCentred", &DocumentWindow::setTitleBarTextCentred)
    //.def ("setMenuBar", &DocumentWindow::setMenuBar)
        .def ("getMenuBarComponent", &DocumentWindow::getMenuBarComponent, py::return_value_policy::reference)
        .def ("setMenuBarComponent", &DocumentWindow::setMenuBarComponent)
        .def ("closeButtonPressed", &DocumentWindow::closeButtonPressed)
        .def ("minimiseButtonPressed", &DocumentWindow::minimiseButtonPressed)
        .def ("maximiseButtonPressed", &DocumentWindow::maximiseButtonPressed)
        .def ("getCloseButton", &DocumentWindow::getCloseButton, py::return_value_policy::reference)
        .def ("getMinimiseButton", &DocumentWindow::getMinimiseButton, py::return_value_policy::reference)
        .def ("getMaximiseButton", &DocumentWindow::getMaximiseButton, py::return_value_policy::reference)
    ;

    py::enum_<DocumentWindow::TitleBarButtons> (classDocumentWindow, "TitleBarButtons")
        .value("minimiseButton", DocumentWindow::minimiseButton)
        .value("maximiseButton", DocumentWindow::maximiseButton)
        .value("closeButton", DocumentWindow::closeButton)
        .value("allButtons", DocumentWindow::allButtons)
        .export_values();

}

} // namespace jucepy::Bindings
