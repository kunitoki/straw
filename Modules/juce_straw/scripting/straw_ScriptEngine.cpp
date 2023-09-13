/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "straw_ScriptEngine.h"
#include "straw_ScriptBindings.h"
#include "straw_ScriptException.h"

namespace straw {

namespace py = pybind11;

//=================================================================================================

inline py::scoped_interpreter& getMainPythonEngine()
{
    static py::scoped_interpreter pythonEngine;
    return pythonEngine;
}

//=================================================================================================

ScriptEngine::ScriptEngine(juce::Array<juce::String> modules)
    : pythonEngine (getMainPythonEngine())
    , customModules (std::move (modules))
{
    py::set_shared_data ("_ENGINE", this);
}

ScriptEngine::~ScriptEngine()
{
}

//=================================================================================================

juce::Result ScriptEngine::runScript (const juce::String& code)
{
    currentScript = code;

    try
    {
        py::gil_scoped_acquire acquire;

        py::dict locals;
        for (const auto& m : customModules)
            locals [m.toRawUTF8()] = py::module_::import (m.toRawUTF8());

        py::exec({ code.toRawUTF8(), code.getNumBytesAsUTF8() }, py::globals(), locals);

        return juce::Result::ok();
    }
    catch (const py::error_already_set& e)
    {
        return juce::Result::fail (e.what());
    }
#if 0 // ! JUCE_DEBUG
    catch (...)
    {
        return juce::Result::fail ("Unhandled exception while processing script");
    }
#endif
}

} // namespace straw
