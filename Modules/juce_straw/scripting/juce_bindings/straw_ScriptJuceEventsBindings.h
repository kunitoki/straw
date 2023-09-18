/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_events/juce_events.h>

#include "../pybind11/embed.h"

//=================================================================================================

namespace straw::Bindings {

void registerJuceEventsBindings (pybind11::module_& m);

} // namespace straw::Bindings
