/**
 * straw 4 the juce - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_core/juce_core.h>

#include <memory>

namespace straw {

//=================================================================================================

struct Request
{
    Request() = default;

    std::shared_ptr<juce::StreamingSocket> connection;
    juce::String verb;
    juce::String path;
    int contentLength = 0;
    juce::String contentType;
    juce::String contentData;
    juce::var data;
};

} // namespace straw
