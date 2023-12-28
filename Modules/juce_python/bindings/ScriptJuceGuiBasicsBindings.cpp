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

namespace jucepy::Bindings {

void registerJuceGuiBasicsBindings (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    // ============================================================================================ juce::JUCEApplication

    py::class_<JUCEApplication, JUCEApplicationBase> (m, "JUCEApplication")
        .def_static ("getInstance", &JUCEApplication::getInstance)
    ;

    // ============================================================================================ juce::Component

    py::class_<Component> (m, "Component")
        .def ("getName", &Component::getName)
        .def ("getComponentID", &Component::getComponentID)
        .def ("isVisible", &Component::isVisible)
        .def ("isShowing", &Component::isShowing)
        .def ("isOnDesktop", &Component::isOnDesktop)
        .def ("getDesktopScaleFactor", &Component::getDesktopScaleFactor)
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
        .def ("getTransform", &Component::getTransform)
        .def ("isTransformed", &Component::isTransformed)
        .def ("getParentWidth", &Component::getParentWidth)
        .def ("getParentHeight", &Component::getParentHeight)
        .def ("getParentMonitorArea", &Component::getParentMonitorArea)
        .def ("getNumChildComponents", &Component::getNumChildComponents)
        .def ("getParentComponent", &Component::getParentComponent, py::return_value_policy::reference)
        .def ("getTopLevelComponent", &Component::getTopLevelComponent, py::return_value_policy::reference)
        .def ("isPaintingUnclipped", &Component::isPaintingUnclipped)
        //.def ("getComponentEffect", &Component::getComponentEffect)
        //.def ("getLookAndFeel", &Component::getLookAndFeel)
        .def ("isOpaque", &Component::isOpaque)
        .def ("isBroughtToFrontOnMouseClick", &Component::isBroughtToFrontOnMouseClick)
        .def ("getExplicitFocusOrder", &Component::getExplicitFocusOrder)
        .def ("isFocusContainer", &Component::isFocusContainer)
        .def ("isKeyboardFocusContainer", &Component::isKeyboardFocusContainer)
        //.def ("findFocusContainer", &Component::findFocusContainer)
        //.def ("findKeyboardFocusContainer", &Component::findKeyboardFocusContainer)
        .def ("getWantsKeyboardFocus", &Component::getWantsKeyboardFocus)
        .def ("getMouseClickGrabsKeyboardFocus", &Component::getMouseClickGrabsKeyboardFocus)
        //.def ("getCurrentlyFocusedComponent", &Component::getCurrentlyFocusedComponent)
        .def ("hasFocusOutline", &Component::hasFocusOutline)
        .def ("isEnabled", &Component::isEnabled)
        .def ("getAlpha", &Component::getAlpha)
        //.def ("getMouseCursor", &Component::getMouseCursor)
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
}

} // namespace jucepy::Bindings
