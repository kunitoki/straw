# ==============================================================================
#
#   This file is part of the straw project.
#   Copyright (c) 2024 - kunitoki@gmail.com
#
#   straw is an open source library subject to open-source licensing.
#
#   The code included in this file is provided under the terms of the ISC license
#   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
#   To use, copy, modify, and/or distribute this software for any purpose with or
#   without fee is hereby granted provided that the above copyright notice and
#   this permission notice appear in all copies.
#
#   STRAW IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
#   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
#   DISCLAIMED.
#
# ==============================================================================

juce_add_modules(juce_straw)

add_library(juce_straw_recommended_warning_flags INTERFACE)
if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"))
    #target_compile_options(juce_straw_recommended_warning_flags INTERFACE "/W4")
elseif((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    target_compile_options(juce_straw_recommended_warning_flags INTERFACE
        -Wno-shadow-field-in-constructor
        -Wno-shadow-field
        -Wno-shadow-uncaptured-local
        -Wno-self-assign-overloaded
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-float-equal)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(juce_straw_recommended_warning_flags INTERFACE
        -Wno-shadow-field-in-constructor
        -Wno-shadow-field
        -Wno-shadow-uncaptured-local
        -Wno-self-assign-overloaded
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-float-equal)
endif()

add_library(straw::juce_straw ALIAS juce_straw)
add_library(straw::juce_straw_recommended_warning_flags ALIAS juce_straw_recommended_warning_flags)
