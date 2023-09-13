/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_core/juce_core.h>

#include <exception>

namespace straw {

//=================================================================================================

class ScriptException : public std::exception
{
public:
    ScriptException (juce::String msg)
        : message (std::move (msg))
    {
    }

    const char* what() const noexcept override
    {
        return message.toUTF8();
    }

private:
    juce::String message;
};

} // namespace straw
