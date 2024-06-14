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

cmake_minimum_required (VERSION 3.28)

project (straw VERSION 0.0.1 LANGUAGES CXX
         DESCRIPTION "JUCE module for automated integration testing"
         HOMEPAGE_URL "https://github.com/kunitoki/straw")

set (PROJECT_NAME straw)
project (${PROJECT_NAME})

add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/Demo")
