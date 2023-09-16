/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../pybind11/embed.h"

//=================================================================================================

namespace straw::Bindings {

void registerJuceGuiBasicsBindings (pybind11::module_& m);

} // namespace straw::Bindings
