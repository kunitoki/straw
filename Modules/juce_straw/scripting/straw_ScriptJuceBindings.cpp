/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "straw_ScriptJuceBindings.h"
#include "straw_ScriptBindings.h"
#include "straw_ScriptException.h"

#include "../values/straw_VariantConverter.h"
#include "../helpers/straw_ComponentHelpers.h"

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "pybind11/operators.h"

#include <functional>
#include <string_view>
#include <tuple>

namespace py = pybind11;

namespace PYBIND11_NAMESPACE {

//=================================================================================================

template <>
struct polymorphic_type_hook<juce::Component>
{
    static const void* get (const juce::Component* src, const std::type_info*& type)
    {
        if (src == nullptr)
            return src;

        auto& map = straw::Bindings::getComponentTypeMap();
        auto demangledName = straw::Helpers::demangleClassName (typeid(*src).name());

        auto it = map.typeMap.find (demangledName);
        if (it != map.typeMap.end())
            return it->second (src, type);

        return src;
    }
};

namespace detail {

//=================================================================================================

bool type_caster<juce::StringRef>::load (handle src, bool)
{
    PyObject* source = src.ptr();

    auto baseType = Py_TYPE (source)->tp_base;
    if (baseType == &PyUnicode_Type)
    {
        Py_ssize_t size = -1;
        const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (source, &size));
        if (buffer == nullptr)
            return false;

        value = buffer;
        return true;
    }

    return false;
}

handle type_caster<juce::StringRef>::cast (const juce::StringRef& src, return_value_policy policy, handle parent)
{
    juce::ignoreUnused (policy, parent);

    return PyUnicode_FromStringAndSize (static_cast<const char*> (src.text), static_cast<Py_ssize_t> (src.length()));
}

//=================================================================================================

bool type_caster<juce::String>::load (handle src, bool)
{
    PyObject* source = src.ptr();

    auto baseType = Py_TYPE (source)->tp_base;
    if (baseType == &PyUnicode_Type)
    {
        Py_ssize_t size = -1;
        const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (source, &size));
        if (buffer == nullptr)
            return false;

        value = juce::String::fromUTF8 (buffer, static_cast<int> (size));
        return true;
    }

    return false;
}

handle type_caster<juce::String>::cast (const juce::String& src, return_value_policy policy, handle parent)
{
    juce::ignoreUnused (policy, parent);

    return PyUnicode_FromStringAndSize (src.toRawUTF8(), static_cast<Py_ssize_t> (src.getNumBytesAsUTF8()));
}

//=================================================================================================

bool type_caster<juce::Identifier>::load (handle src, bool)
{
    PyObject* source = src.ptr();

    auto baseType = Py_TYPE (source)->tp_base;
    if (baseType == &PyUnicode_Type)
    {
        Py_ssize_t size = -1;
        const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (source, &size));
        if (! buffer)
            return false;

        value = juce::Identifier (juce::String::fromUTF8 (buffer, static_cast<int> (size)));
        return true;
    }

    return false;
}

handle type_caster<juce::Identifier>::cast (const juce::Identifier& src, return_value_policy policy, handle parent)
{
    juce::ignoreUnused (policy, parent);

    return make_caster<juce::String>::cast (src.toString(), policy, parent);
}

//=================================================================================================

bool type_caster<juce::var>::load (handle src, bool)
{
    PyObject* source = src.ptr();

    auto baseType = Py_TYPE (source)->tp_base;
    if (baseType == &PyBool_Type)
        value = PyObject_IsTrue (source) ? true : false;

    else if (baseType == &PyLong_Type)
        value = static_cast<int> (PyLong_AsLong (source));

    else if (baseType == &PyFloat_Type)
        value = PyFloat_AsDouble (source);

    else if (baseType == &PyUnicode_Type)
        value = juce::String::fromUTF8 (PyUnicode_AsUTF8 (source));

    else
        value = juce::var::undefined();

    // TODO - raise

    return !PyErr_Occurred();
}

handle type_caster<juce::var>::cast (const juce::var& src, return_value_policy policy, handle parent)
{
    if (src.isBool())
        return PyBool_FromLong (static_cast<bool> (src));

    else if (src.isInt())
        return PyLong_FromLong (static_cast<int> (src));

    else if (src.isInt64())
        return PyLong_FromLongLong (static_cast<int64_t> (src));

    else if (src.isDouble())
        return PyFloat_FromDouble (static_cast<double> (src));

    else if (src.isString())
        return make_caster<juce::String>::cast (src, policy, parent);

    else if (src.isVoid() || src.isUndefined())
        return Py_None;

    else if (src.isObject())
    {
        auto object = src.getDynamicObject();
        if (object == nullptr)
            return Py_None;

        py::object result;

        for (const auto& props : object->getProperties())
            result [props.name.toString().toRawUTF8()] = props.value;

        return result;
    }

    else if (src.isArray())
    {
        py::list list;

        auto array = src.getArray();
        for (const auto& value : *array)
            list.append (value);

        return list;
    }

    else if (src.isBinaryData())
    {
        auto data = src.getBinaryData();
        return py::bytes (
            static_cast<const char*> (data->getData()),
            static_cast<Py_ssize_t> (data->getSize()));
    }

    else if (src.isMethod())
    {
        return py::cpp_function ([src]
        {
            juce::var::NativeFunctionArgs args (juce::var(), nullptr, 0);
            return src.getNativeFunction() (args);
        });
    }

    return Py_None;
}

//=================================================================================================

bool type_caster<juce::NamedValueSet>::load (handle src, bool convert)
{
    if (!isinstance<dict>(src))
        return false;

    value.clear();

    auto d = reinterpret_borrow<dict>(src);
    for (auto it : d)
    {
        make_caster<juce::Identifier> kconv;
        make_caster<juce::var> vconv;

        if (!kconv.load(it.first.ptr(), convert) || !vconv.load(it.second.ptr(), convert))
            return false;

        value.set (cast_op<juce::Identifier&&> (std::move (kconv)), cast_op<juce::var&&> (std::move (vconv)));
    }

    return true;
}

handle type_caster<juce::NamedValueSet>::cast (const juce::NamedValueSet& src, return_value_policy policy, handle parent)
{
    dict d;

    for (const auto& kv : src)
    {
        auto key = reinterpret_steal<object> (make_caster<juce::Identifier>::cast (kv.name, policy, parent));
        auto value = reinterpret_steal<object> (make_caster<juce::var>::cast (kv.value, policy, parent));

        if (!key || !value)
            return handle();

        d [std::move (key)] = std::move (value);
    }

    return d.release();
}

}} // namespace PYBIND11_NAMESPACE::detail

//=================================================================================================

PYBIND11_EMBEDDED_MODULE(juce, m)
{
    using namespace juce;
    using namespace straw;

    // ============================================================================================ juce::Point<>

    py::class_<Point<int>> (m, "PointInt")
        .def (py::init<>())
        /*
        .def (py::init<const Point<int>&>())
        .def (py::init<int, int>())
        */
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isOrigin", &Point<int>::isOrigin)
        .def ("isFinite", &Point<int>::isFinite)
        .def ("getX", &Point<int>::getX)
        .def ("getY", &Point<int>::getY)
        .def ("setX", &Point<int>::setX)
        .def ("setY", &Point<int>::setY)
        .def ("withX", &Point<int>::withX)
        .def ("withY", &Point<int>::withY)
        .def ("setXY", &Point<int>::setXY)
        .def ("addXY", &Point<int>::addXY)
        .def ("translated", &Point<int>::translated)
        .def (py::self + py::self)
        .def (py::self += py::self)
        .def (py::self - py::self)
        .def (py::self -= py::self)
        .def (py::self * py::self)
        .def (py::self *= py::self)
        .def (py::self * float())
        .def (py::self *= float())
        .def (py::self / py::self)
        .def (py::self /= py::self)
        .def (py::self / float())
        .def (py::self /= float())
        .def (-py::self)
        .def ("getDistanceFromOrigin", &Point<int>::getDistanceFromOrigin)
        .def ("getDistanceFrom", &Point<int>::getDistanceFrom)
        .def ("getDistanceSquaredFromOrigin", &Point<int>::getDistanceSquaredFromOrigin)
        .def ("getDistanceSquaredFrom", &Point<int>::getDistanceSquaredFrom)
        .def ("getAngleToPoint", &Point<int>::getAngleToPoint)
        .def ("rotatedAboutOrigin", &Point<int>::rotatedAboutOrigin)
        .def ("getPointOnCircumference", py::overload_cast<float, float>(&Point<int>::getPointOnCircumference, py::const_))
        .def ("getPointOnCircumference", py::overload_cast<float, float, float>(&Point<int>::getPointOnCircumference, py::const_))
        .def ("getDotProduct", &Point<int>::getDotProduct)
        //.def ("applyTransform", &Point<int>::applyTransform)
        //.def ("transformedBy", &Point<int>::transformedBy)
        .def ("toInt", &Point<int>::toInt)
        .def ("toFloat", &Point<int>::toFloat)
        .def ("toDouble", &Point<int>::toDouble)
        .def ("roundToInt", &Point<int>::roundToInt)
        .def ("toString", &Point<int>::toString)
        .def_property("x", &Point<int>::getX, &Point<int>::setX)
        .def_property("y", &Point<int>::getY, &Point<int>::setY)
    ;

    py::class_<Point<float>> (m, "PointFloat")
        .def (py::init<>())
        /*
        .def (py::init<const Point<float>&>())
        .def (py::init<float, float>())
        */
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isOrigin", &Point<float>::isOrigin)
        .def ("isFinite", &Point<float>::isFinite)
        .def ("getX", &Point<float>::getX)
        .def ("getY", &Point<float>::getY)
        .def ("setX", &Point<float>::setX)
        .def ("setY", &Point<float>::setY)
        .def ("withX", &Point<float>::withX)
        .def ("withY", &Point<float>::withY)
        .def ("setXY", &Point<float>::setXY)
        .def ("addXY", &Point<float>::addXY)
        .def ("translated", &Point<float>::translated)
        .def (py::self + py::self)
        .def (py::self += py::self)
        .def (py::self - py::self)
        .def (py::self -= py::self)
        .def (py::self * py::self)
        .def (py::self *= py::self)
        .def (py::self * float())
        .def (py::self *= float())
        .def (py::self / py::self)
        .def (py::self /= py::self)
        .def (py::self / float())
        .def (py::self /= float())
        .def (-py::self)
        .def ("getDistanceFromOrigin", &Point<float>::getDistanceFromOrigin)
        .def ("getDistanceFrom", &Point<float>::getDistanceFrom)
        .def ("getDistanceSquaredFromOrigin", &Point<float>::getDistanceSquaredFromOrigin)
        .def ("getDistanceSquaredFrom", &Point<float>::getDistanceSquaredFrom)
        .def ("getAngleToPoint", &Point<float>::getAngleToPoint)
        .def ("rotatedAboutOrigin", &Point<float>::rotatedAboutOrigin)
        .def ("getPointOnCircumference", py::overload_cast<float, float>(&Point<float>::getPointOnCircumference, py::const_))
        .def ("getPointOnCircumference", py::overload_cast<float, float, float>(&Point<float>::getPointOnCircumference, py::const_))
        .def ("getDotProduct", &Point<float>::getDotProduct)
        //.def ("applyTransform", &Point<float>::applyTransform)
        //.def ("transformedBy", &Point<float>::transformedBy)
        .def ("toInt", &Point<float>::toInt)
        .def ("toFloat", &Point<float>::toFloat)
        .def ("toDouble", &Point<float>::toDouble)
        .def ("roundToInt", &Point<float>::roundToInt)
        .def ("toString", &Point<float>::toString)
        .def_property("x", &Point<float>::getX, &Point<float>::setX)
        .def_property("y", &Point<float>::getY, &Point<float>::setY)
    ;

    // ============================================================================================ juce::Rectangle<>

    py::class_<Rectangle<int>> (m, "RectangleInt")
        .def ("isEmpty", &Rectangle<int>::isEmpty)
        .def ("isFinite", &Rectangle<int>::isFinite)
        .def ("getX", &Rectangle<int>::getX)
        .def ("getY", &Rectangle<int>::getY)
        .def ("getWidth", &Rectangle<int>::getWidth)
        .def ("getHeight", &Rectangle<int>::getHeight)
        .def ("getRight", &Rectangle<int>::getRight)
        .def ("getBottom", &Rectangle<int>::getBottom)
        .def ("getCentreX", &Rectangle<int>::getCentreX)
        .def ("getCentreX", &Rectangle<int>::getCentreX)
        .def ("getAspectRatio", &Rectangle<int>::getAspectRatio)
        .def ("getPosition", &Rectangle<int>::getPosition)
        .def ("getTopLeft", &Rectangle<int>::getTopLeft)
        .def ("getTopRight", &Rectangle<int>::getTopRight)
        .def ("getBottomLeft", &Rectangle<int>::getBottomLeft)
        .def ("getBottomRight", &Rectangle<int>::getBottomRight)
    ;

    py::class_<Rectangle<float>> (m, "RectangleFloat")
        .def ("isEmpty", &Rectangle<float>::isEmpty)
        .def ("isFinite", &Rectangle<float>::isFinite)
        .def ("getX", &Rectangle<float>::getX)
        .def ("getY", &Rectangle<float>::getY)
        .def ("getWidth", &Rectangle<float>::getWidth)
        .def ("getHeight", &Rectangle<float>::getHeight)
        .def ("getRight", &Rectangle<float>::getRight)
        .def ("getBottom", &Rectangle<float>::getBottom)
        .def ("getCentreX", &Rectangle<float>::getCentreX)
        .def ("getCentreX", &Rectangle<float>::getCentreX)
        .def ("getAspectRatio", &Rectangle<float>::getAspectRatio)
        .def ("getPosition", &Rectangle<float>::getPosition)
        .def ("getTopLeft", &Rectangle<float>::getTopLeft)
        .def ("getTopRight", &Rectangle<float>::getTopRight)
        .def ("getBottomLeft", &Rectangle<float>::getBottomLeft)
        .def ("getBottomRight", &Rectangle<float>::getBottomRight)
    ;

    // ============================================================================================ juce::PixelARGB

    py::class_<PixelARGB> (m, "PixelARGB")
        .def (py::init<>())
        //.def (py::init<uint8, uint8, uint8, uint8>())
        .def ("getNativeARGB", &PixelARGB::getNativeARGB)
        .def ("getInARGBMaskOrder", &PixelARGB::getInARGBMaskOrder)
        .def ("getInARGBMemoryOrder", &PixelARGB::getInARGBMemoryOrder)
        .def ("getEvenBytes", &PixelARGB::getEvenBytes)
        .def ("getOddBytes", &PixelARGB::getOddBytes)
        .def ("getAlpha", &PixelARGB::getAlpha)
        .def ("getRed", &PixelARGB::getRed)
        .def ("getGreen", &PixelARGB::getGreen)
        .def ("getBlue", &PixelARGB::getBlue)
        //.def ("set", &PixelARGB::set<?>)
        .def ("setARGB", &PixelARGB::setARGB)
        //.def ("blend", &PixelARGB::blend<?>)
        //.def ("blend", py::overload_cast<PixelRGB>(&PixelARGB::blend))
        //.def ("tween", &PixelARGB::tween<?>)
        .def ("setAlpha", &PixelARGB::setAlpha)
        //.def ("multiplyAlpha", py::overload_cast<float>(&PixelARGB::multiplyAlpha, py::const_))
        .def ("getUnpremultiplied", &PixelARGB::getUnpremultiplied)
        .def ("premultiply", &PixelARGB::premultiply)
        .def ("unpremultiply", &PixelARGB::unpremultiply)
        .def ("desaturate", &PixelARGB::desaturate)
    ;

    py::class_<PixelRGB> (m, "PixelRGB")
        .def (py::init<>())
        .def ("getNativeARGB", &PixelRGB::getNativeARGB)
        .def ("getInARGBMaskOrder", &PixelRGB::getInARGBMaskOrder)
        .def ("getInARGBMemoryOrder", &PixelRGB::getInARGBMemoryOrder)
        .def ("getEvenBytes", &PixelRGB::getEvenBytes)
        .def ("getOddBytes", &PixelRGB::getOddBytes)
        .def ("getAlpha", &PixelRGB::getAlpha)
        .def ("getRed", &PixelRGB::getRed)
        .def ("getGreen", &PixelRGB::getGreen)
        .def ("getBlue", &PixelRGB::getBlue)
        //.def ("set", &PixelRGB::set<?>)
        .def ("setARGB", &PixelRGB::setARGB)
        //.def ("blend", &PixelRGB::blend<?>)
        //.def ("blend", py::overload_cast<PixelRGB>(&PixelRGB::blend))
        //.def ("tween", &PixelRGB::tween<?>)
        .def ("setAlpha", &PixelRGB::setAlpha)
        //.def ("multiplyAlpha", py::overload_cast<float>(&PixelRGB::multiplyAlpha, py::const_))
        .def ("premultiply", &PixelRGB::premultiply)
        .def ("unpremultiply", &PixelRGB::unpremultiply)
        .def ("desaturate", &PixelRGB::desaturate)
    ;

    py::class_<PixelAlpha> (m, "PixelAlpha")
        .def (py::init<>())
        .def ("getNativeARGB", &PixelAlpha::getNativeARGB)
        .def ("getInARGBMaskOrder", &PixelAlpha::getInARGBMaskOrder)
        .def ("getInARGBMemoryOrder", &PixelAlpha::getInARGBMemoryOrder)
        .def ("getEvenBytes", &PixelAlpha::getEvenBytes)
        .def ("getOddBytes", &PixelAlpha::getOddBytes)
        .def ("getAlpha", &PixelAlpha::getAlpha)
        .def ("getRed", &PixelAlpha::getRed)
        .def ("getGreen", &PixelAlpha::getGreen)
        .def ("getBlue", &PixelAlpha::getBlue)
        //.def ("set", &PixelAlpha::set<?>)
        .def ("setARGB", &PixelAlpha::setARGB)
        //.def ("blend", &PixelAlpha::blend<?>)
        //.def ("blend", &PixelAlpha::blend<PixelRGB>)
        //.def ("tween", &PixelAlpha::tween<?>)
        .def ("setAlpha", &PixelAlpha::setAlpha)
        //.def ("multiplyAlpha", py::overload_cast<float>(&PixelAlpha::multiplyAlpha, py::const_))
        .def ("premultiply", &PixelAlpha::premultiply)
        .def ("unpremultiply", &PixelAlpha::unpremultiply)
        .def ("desaturate", &PixelAlpha::desaturate)
    ;

    // ============================================================================================ juce::Colour

    py::class_<Colour> (m, "Colour")
        .def (py::init<>())
        /*
        .def (py::init<uint32>())
        .def (py::init<uint8, uint8, uint8>())
        .def (py::init<uint8, uint8, uint8, uint8>())
        .def (py::init<uint8, uint8, uint8, float>())
        .def (py::init<float, float, float, uint8>())
        .def (py::init<float, float, float, float>())
        .def (py::init<PixelARGB>())
        .def (py::init<PixelRGB>())
        .def (py::init<PixelAlpha>())
        */
        .def_static ("fromRGB", &Colour::fromRGB)
        .def_static ("fromRGBA", &Colour::fromRGBA)
        .def_static ("fromFloatRGBA", &Colour::fromFloatRGBA)
        .def_static ("fromHSV", &Colour::fromHSV)
        .def_static ("fromHSL", &Colour::fromHSL)
        .def ("getRed", &Colour::getRed)
        .def ("getGreen", &Colour::getGreen)
        .def ("getBlue", &Colour::getBlue)
        .def ("getFloatRed", &Colour::getFloatRed)
        .def ("getFloatGreen", &Colour::getFloatGreen)
        .def ("getFloatBlue", &Colour::getFloatBlue)
        .def ("getPixelARGB", &Colour::getPixelARGB)
        .def ("getNonPremultipliedPixelARGB", &Colour::getNonPremultipliedPixelARGB)
        .def ("getARGB", &Colour::getARGB)
        .def ("getAlpha", &Colour::getAlpha)
        .def ("getFloatAlpha", &Colour::getFloatAlpha)
        .def ("isOpaque", &Colour::isOpaque)
        .def ("isTransparent", &Colour::isTransparent)
        .def ("withAlpha", py::overload_cast<float>(&Colour::withAlpha, py::const_))
        .def ("withMultipliedAlpha", &Colour::withMultipliedAlpha)
        .def ("overlaidWith", &Colour::overlaidWith)
        .def ("interpolatedWith", &Colour::interpolatedWith)
        .def ("getHue", &Colour::getHue)
        .def ("getSaturation", &Colour::getSaturation)
        .def ("getSaturationHSL", &Colour::getSaturationHSL)
        .def ("getBrightness", &Colour::getBrightness)
        .def ("getLightness", &Colour::getLightness)
        .def ("getPerceivedBrightness", &Colour::getPerceivedBrightness)
        .def ("withHue", &Colour::withHue)
        .def ("withSaturation", &Colour::withSaturation)
        .def ("withSaturationHSL", &Colour::withSaturationHSL)
        .def ("withBrightness", &Colour::withBrightness)
        .def ("withLightness", &Colour::withLightness)
        .def ("withRotatedHue", &Colour::withRotatedHue)
        .def ("withMultipliedSaturation", &Colour::withMultipliedSaturation)
        .def ("withMultipliedSaturationHSL", &Colour::withMultipliedSaturationHSL)
        .def ("withMultipliedBrightness", &Colour::withMultipliedBrightness)
        .def ("withMultipliedLightness", &Colour::withMultipliedLightness)
        .def ("brighter", &Colour::brighter)
        .def ("darker", &Colour::darker)
        .def ("contrasting", py::overload_cast<float>(&Colour::contrasting, py::const_))
        .def_static ("contrasting", py::overload_cast<Colour, Colour>(&Colour::contrasting))
        .def_static ("greyLevel", &Colour::greyLevel)
        .def ("toString", &Colour::toString)
        .def_static ("fromString", &Colour::fromString)
        .def ("toDisplayString", &Colour::toDisplayString)
    ;

    // ============================================================================================ juce::Image

    py::class_<Image> imageClass (m, "Image");

    imageClass
        .def (py::init<>())
        /*
        .def (py::init<Image::PixelFormat, int, int, bool>())
        .def (py::init<const Image&>())
        */
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isValid", &Image::isValid)
        .def ("isNull", &Image::isNull)
        .def ("getWidth", &Image::getWidth)
        .def ("getHeight", &Image::getHeight)
        .def ("getBounds", &Image::getBounds)
        .def ("getFormat", &Image::getFormat)
        .def ("isARGB", &Image::isARGB)
        .def ("isRGB", &Image::isRGB)
        .def ("isSingleChannel", &Image::isSingleChannel)
        .def ("hasAlphaChannel", &Image::hasAlphaChannel)
        .def ("clear", &Image::clear)
        //.def ("rescaled", &Image::rescaled)
        .def ("createCopy", &Image::createCopy)
        .def ("convertedToFormat", &Image::convertedToFormat)
        .def ("duplicateIfShared", &Image::duplicateIfShared)
        .def ("getClippedImage", &Image::getClippedImage)
        .def ("getPixelAt", &Image::getPixelAt)
        .def ("setPixelAt", &Image::setPixelAt)
        .def ("multiplyAlphaAt", &Image::multiplyAlphaAt)
        .def ("multiplyAllAlphas", &Image::multiplyAllAlphas)
        .def ("desaturate", &Image::desaturate)
        .def ("moveImageSection", &Image::moveImageSection)
        //.def ("createSolidAreaMask", &Image::createSolidAreaMask)
        //.def ("getProperties", &Image::getProperties)
        //.def ("createLowLevelContext", &Image::createLowLevelContext)
        .def ("getReferenceCount", &Image::getReferenceCount)
        //.def ("getPixelData", &Image::getPixelData)
    ;

    py::enum_<Image::PixelFormat> (imageClass, "Image")
        .value("UnknownFormat", Image::PixelFormat::UnknownFormat)
        .value("RGB", Image::PixelFormat::RGB)
        .value("ARGB", Image::PixelFormat::ARGB)
        .value("SingleChannel", Image::PixelFormat::SingleChannel)
        .export_values();

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
        //.def ("getTransform", &Component::getTransform)
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
