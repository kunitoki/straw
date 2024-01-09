/**
 * juce python - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#if __has_include(<juce_events/juce_events.h>)

#include <juce_events/juce_events.h>

#include "../pybind11/embed.h"

//=================================================================================================

namespace jucepy::Bindings {

void registerJuceEventsBindings (pybind11::module_& m);

} // namespace jucepy::Bindings

#endif
