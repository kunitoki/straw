/**
 * juce python - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#if __has_include(<juce_gui_basics/juce_gui_basics.h>)

#include <juce_gui_basics/juce_gui_basics.h>

#include "../pybind11/embed.h"

//=================================================================================================

namespace jucepy::Bindings {

void registerJuceGuiBasicsBindings (pybind11::module_& m);

} // namespace jucepy::Bindings

#endif
