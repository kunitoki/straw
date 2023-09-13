/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_core/juce_core.h>

#include "pybind11/embed.h"

#include <functional>

namespace straw {

//=================================================================================================

class ScriptEngine;

//=================================================================================================

class ScriptEngine
{
public:
    ScriptEngine(juce::Array<juce::String> modules);
    ~ScriptEngine();

    juce::Result runScript (const juce::String& code);
        
private:
    pybind11::scoped_interpreter& pythonEngine;
    juce::Array<juce::String> customModules;
    juce::String currentScript;

    JUCE_DECLARE_WEAK_REFERENCEABLE (ScriptEngine)
};

} // namespace straw
