/*
  ==============================================================================

   This file is part of the straw project.
   Copyright (c) 2024 - kunitoki@gmail.com

   straw is an open source library subject to open-source licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   STRAW IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
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
