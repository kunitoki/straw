/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "straw_ScriptJuceCoreBindings.h"
#include "../straw_ScriptBindings.h"
#include "../straw_ScriptException.h"

#include "../../values/straw_VariantConverter.h"

namespace PYBIND11_NAMESPACE {
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
        auto dynamicObject = src.getDynamicObject();
        if (dynamicObject == nullptr)
            return Py_None;

        object result;

        for (const auto& props : dynamicObject->getProperties())
            result [props.name.toString().toRawUTF8()] = props.value;

        return result;
    }

    else if (src.isArray())
    {
        list list;

        auto array = src.getArray();
        for (const auto& value : *array)
            list.append (value);

        return list;
    }

    else if (src.isBinaryData())
    {
        auto data = src.getBinaryData();
        return bytes (
            static_cast<const char*> (data->getData()),
            static_cast<Py_ssize_t> (data->getSize()));
    }

    else if (src.isMethod())
    {
        return cpp_function ([src]
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

namespace straw::Bindings {

void registerJuceCoreBindings ([[maybe_unused]] pybind11::module_& m)
{
    using namespace juce;
    namespace py = pybind11;

    // TODO
}

} // namespace straw::Bindings
