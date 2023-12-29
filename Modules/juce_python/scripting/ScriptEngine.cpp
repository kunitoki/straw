/**
 * juce python - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "ScriptEngine.h"
#include "ScriptBindings.h"
#include "ScriptException.h"

#include <regex>

namespace jucepy {

namespace py = pybind11;

//=================================================================================================

namespace {

[[maybe_unused]] juce::String replaceBrokenLineNumbers (const juce::String& input, const juce::String& code)
{
    static const std::regex pattern ("<string>\\((\\d+)\\)");

    const auto codeLines = juce::StringArray::fromLines (code);

    juce::String output;
    std::string result = input.toStdString();
    std::ptrdiff_t startPos = 0;

    std::smatch match;
    while (std::regex_search (result, match, pattern))
    {
        if (match.size() > 1)
        {
            const int matchLine = std::stoi (match[1]) - 1;

            output
                << input.substring (static_cast<int> (startPos), static_cast<int> (match.position() - startPos))
                << "<string>(" << matchLine << "): \'" << codeLines[matchLine - 1] << "\'";

            startPos = match.position() + match.length();
        }

        result = match.suffix();
    }

    output << result;
    return output;
}

} // namespace

//=================================================================================================

inline py::scoped_interpreter& getMainPythonEngine()
{
    static py::scoped_interpreter pythonEngine;
    return pythonEngine;
}

//=================================================================================================

ScriptEngine::ScriptEngine (juce::StringArray modules)
    : pythonEngine (getMainPythonEngine())
    , customModules (std::move (modules))
{
    py::set_shared_data ("_ENGINE", this);
}

ScriptEngine::~ScriptEngine()
{
    py::set_shared_data ("_ENGINE", nullptr);
}

//=================================================================================================

juce::Result ScriptEngine::runScript (const juce::String& code)
{
    currentScriptCode = code;
    currentScriptFile = juce::File();

    return runScriptInternal (currentScriptCode);
}

//=================================================================================================

juce::Result ScriptEngine::runScript (const juce::File& script)
{
    {
        auto is = script.createInputStream();
        if (is == nullptr)
            return juce::Result::fail ("Unable to open the requested script file");

        currentScriptCode = is->readEntireStreamAsString();
        currentScriptFile = script;
    }

    return runScriptInternal (currentScriptCode);
}

//=================================================================================================

juce::Result ScriptEngine::runScriptInternal (const juce::String& code)
{
#if JUCE_PYTHON_SCRIPT_CATCH_EXCEPTION
    try
#endif

    {
        py::gil_scoped_acquire acquire;

        py::dict globals, locals;
        for (const auto& m : customModules)
            globals [m.toRawUTF8()] = py::module_::import (m.toRawUTF8());

        py::str pythonCode { code.toRawUTF8(), code.getNumBytesAsUTF8() };

        py::exec (std::move (pythonCode), globals, locals);

        return juce::Result::ok();
    }

#if JUCE_PYTHON_SCRIPT_CATCH_EXCEPTION
    catch (const py::error_already_set& e)
    {
        return juce::Result::fail (replaceBrokenLineNumbers (e.what(), code));
    }
    catch (...)
    {
        return juce::Result::fail ("Unhandled exception while processing script");
    }
#endif
}

} // namespace jucepy