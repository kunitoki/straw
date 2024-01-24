/**
 * straw 4 the juce - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

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

#pragma once

#include <juce_python/juce_python.h>

#include "server/straw_Request.h"
#include "server/straw_AutomationServer.h"
#include "helpers/straw_ComponentHelpers.h"
#include "values/straw_VariantConverter.h"
#include "center/straw_TestCenter.h"
