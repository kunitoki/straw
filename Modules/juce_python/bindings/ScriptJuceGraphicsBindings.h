/**
 * juce python - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_graphics/juce_graphics.h>

#include "../pybind11/embed.h"

//=================================================================================================

namespace jucepy::Bindings {

void registerJuceGraphicsBindings (pybind11::module_& m);

} // namespace jucepy::Bindings
