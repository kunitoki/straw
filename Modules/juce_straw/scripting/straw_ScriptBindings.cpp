/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "straw_ScriptBindings.h"
#include "straw_ScriptException.h"
#include "straw_ScriptUtilities.h"

#include "../values/straw_VariantConverter.h"
#include "../helpers/straw_ComponentHelpers.h"

#include <functional>
#include <string_view>
#include <tuple>

namespace straw {

//=================================================================================================

namespace Bindings {

ComponentTypeMap& getComponentTypeMap()
{
    static ComponentTypeMap typeMap;
    return typeMap;
}

void registerComponentType (juce::StringRef className, ComponentTypeCaster classCaster)
{
    auto& map = getComponentTypeMap();

    auto lock = juce::CriticalSection::ScopedLockType (map.mutex);
    map.typeMap [className] = std::move (classCaster);
}

void clearComponentTypes()
{
    auto& map = getComponentTypeMap();

    auto lock = juce::CriticalSection::ScopedLockType (map.mutex);
    map.typeMap.clear();
}

} // namespace Bindings

} // namespace straw

//=================================================================================================

PYBIND11_EMBEDDED_MODULE(straw, m)
{
    namespace py = pybind11;

    using namespace juce;
    using namespace straw;

    py::module_::import ("juce");

    py::register_exception<ScriptException>(m, "ScriptException");

    m.def("findComponentById", [](py::args args) -> Component*
    {
        if (args.size() != 1)
            throw ScriptException ("Missing argument componentId when calling findComponentById");

        //auto engine = reinterpret_cast<ScriptEngine*> (py::get_shared_data ("_ENGINE"));
        //if (engine == nullptr)
        //    throw ScriptException ("Missing _ENGINE shared data when calling findComponent");

        if (auto component = Helpers::findComponentById (String (py::str (args [0]))))
            return component;

        return {};
    }, py::return_value_policy::reference);

    m.def("findComponentsByType", [](py::args args)
    {
        if (args.size() != 1)
            throw ScriptException ("Missing argument typeName when calling findComponentByType");

        auto components = Helpers::findComponentsByType (String (py::str (args [0])));
        
        py::list list;
        for (const auto& comp : components)
            list.append (comp);
        return list;
    });

    m.def("clickComponent", [](py::args args)
    {
        if (args.size() != 1)
            throw ScriptException ("Missing argument componentId when calling clickComponent");

        juce::Component* component = python_cast<Component*> (args [0]).value_or (nullptr);
        if (component == nullptr)
            component = Helpers::findComponentById (String (py::str (args [0])));

        if (component != nullptr)
        {
            Helpers::clickComponent (component, {}, nullptr);
            return;
        }
    });

    m.def("renderComponent", [](py::args args)
    {
        if (args.size() == 0)
            throw ScriptException ("Missing argument componentId when calling renderComponent");

        bool withChildren = false;
        if (args.size() > 1)
            withChildren = args [1].cast<bool>();

        auto component = python_cast<Component*> (args [0]).value_or (nullptr);
        if (component == nullptr)
            component = Helpers::findComponentById (String (py::str (args [0])));

        if (component != nullptr)
            return Helpers::renderComponentToImage (component, withChildren);

        return Image();
    });

    m.def ("invokeComponentCustomMethod", [](py::args args) -> juce::var
    {
        if (args.size() < 2)
            throw ScriptException ("Missing arguments componentID and/or methodName when calling invokeComponentCustomMethod");

        auto component = python_cast<Component*> (args [0]).value_or (nullptr);
        if (component == nullptr)
            component = Helpers::findComponentById (String (py::str (args [0])));
            
        if (component != nullptr)
        {
            auto methodName = String (py::str (args [1]));

            Array<var> arguments;
            for (std::size_t i = 2; i < args.size(); ++i)
                arguments.add (args [i].cast<var>());

            return Helpers::invokeComponentCustomMethod(component, methodName, arguments, [](StringRef exception)
            {
                throw ScriptException (exception);
            });
        }

        return juce::var();
    });

    m.def("assertTrue", [](py::args args)
    {
        if (args.size() != 1)
            throw ScriptException ("Invalid number of arguments when calling assertTrue");

        if (! py::object (args[0]))
            throw ScriptException ("Parameter does not evaluate to true");

        return true;
    });

    m.def("assertFalse", [](py::args args)
    {
        if (args.size() != 1)
            throw ScriptException ("Invalid number of arguments when calling assertFalse");

        if (py::object (args[0]))
            throw ScriptException ("Parameter does not evaluate to false");

        return true;
    });

    m.def("assertEqual", [](py::args args)
    {
        if (args.size() != 2)
            throw ScriptException ("Invalid number of arguments when calling assertEqual");

        if (! (py::object (args[0]).equal (py::object (args[1]))))
            throw ScriptException ("Parameters are not equal");

        return true;
    });

    m.def("assertNotEqual", [](py::args args)
    {
        if (args.size() != 2)
            throw straw::ScriptException ("Invalid number of arguments when calling assertNotEqual");

        if (! (py::object (args[0]).not_equal (py::object (args[1]))))
            throw straw::ScriptException ("Parameters are not equal");

        return true;
    });

    m.def("assertLessThan", [](py::args args)
    {
        if (args.size() != 2)
            throw straw::ScriptException ("Invalid number of arguments when calling assertLessThan");

        if (! (py::object (args[0]) < py::object (args[1])))
            throw straw::ScriptException ("Parameter a is not less than b");

        return true;
    });

    m.def("assertLessThanEqual", [](py::args args)
    {
        if (args.size() != 2)
            throw straw::ScriptException ("Invalid number of arguments when calling assertLessThanEqual");

        if (! (py::object (args[0]) <= py::object (args[1])))
            throw straw::ScriptException ("Parameter a is not less than equal b");

        return true;
    });

    m.def("assertGreaterThan", [](py::args args)
    {
        if (args.size() != 2)
            throw straw::ScriptException ("Invalid number of arguments when calling assertGreaterThan");

        if (! (py::object (args[0]) > py::object (args[1])))
            throw straw::ScriptException ("Parameter a is not greater than b");

        return true;
    });

    m.def("assertGreaterThanEqual", [](py::args args)
    {
        if (args.size() != 2)
            throw straw::ScriptException ("Invalid number of arguments when calling assertGreaterThanEqual");

        if (! (py::object (args[0]) >= py::object (args[1])))
            throw straw::ScriptException ("Parameter a is not greater than equal b");

        return true;
    });

    m.def("throwException", [](py::args args)
    {
        String message;
        for (const auto& arg : args)
            message << arg.cast<py::str>();

        message = message.trim();
        if (message.isEmpty())
            message = "unknown script exception";

        throw py::type_error (message.toRawUTF8());
    });

    m.def("quitApplication", []
    {
        juce::JUCEApplication::getInstance()->quit();
    });

    m.def("log", [](py::args args)
    {
        auto currentTime = Time::getCurrentTime();

        String message;
        message
            << JUCEApplication::getInstance()->getApplicationName()
            << currentTime.formatted (" (%Y/%m/%d %H:%M:%S.")
            << currentTime.getMilliseconds()
            << ")> ";

        for (const auto& arg : args)
            message << arg.cast<py::str>();

        Logger::writeToLog (message);
    });
}
