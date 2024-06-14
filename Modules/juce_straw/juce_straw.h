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

/*
 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 juce_straw
  vendor:             kunitoki
  version:            0.0.1
  name:               Straws for the JUCE framework
  description:        The automated integration test framework for automating JUCE apps on CI.
  website:            http://www.straw.com
  license:            DUAL
  minimumCppStandard: 17

  dependencies:       juce_core juce_events juce_gui_basics juce_python

 END_JUCE_MODULE_DECLARATION
*/

#include <juce_python/juce_python.h>

#include "server/straw_Request.h"
#include "server/straw_AutomationServer.h"
#include "helpers/straw_ComponentHelpers.h"
#include "values/straw_VariantConverter.h"
#include "center/straw_TestCenter.h"
