/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_core/juce_core.h>

#include "pybind11/embed.h"

#include <functional>

namespace straw {

//=================================================================================================

/**
 * @brief The ScriptEngine class provides a C++ interface for running Python scripts
 *
 * The ScriptEngine class encapsulates the functionality to run Python scripts within a C++ application. It utilizes the pybind11 library to
 * interact with the Python interpreter and provides a way to run custom Python code within the C++ environment.
 */
class ScriptEngine
{
public:
    /**
     * @brief Construct a new ScriptEngine object
     *
     * Initializes a ScriptEngine object with the specified custom modules.
     *
     * @param modules An array of module names to be imported in the Python interpreter.
     *
     * @warning Ensure that the provided modules are available and compatible with the Python interpreter.
     */
    ScriptEngine(juce::Array<juce::String> modules);

    /**
     * @brief Destroy the ScriptEngine object
     *
     * Cleans up resources associated with the ScriptEngine object.
     */
    ~ScriptEngine();

    /**
     * @brief Run a Python script
     *
     * Executes the given Python code within the Python interpreter.
     *
     * @param code The Python code to be executed.
     *
     * @return A Result object indicating the success or failure of the script execution.
     */
    juce::Result runScript (const juce::String& code);
        
private:
    pybind11::scoped_interpreter& pythonEngine;
    juce::Array<juce::String> customModules;
    juce::String currentScript;

    JUCE_DECLARE_WEAK_REFERENCEABLE (ScriptEngine)
};

} // namespace straw
