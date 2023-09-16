/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "straw_ScriptJuceBindings.h"

#include "juce_bindings/straw_ScriptJuceCoreBindings.cpp"
#include "juce_bindings/straw_ScriptJuceGraphicsBindings.cpp"
#include "juce_bindings/straw_ScriptJuceGuiBasicsBindings.cpp"

//=================================================================================================

PYBIND11_EMBEDDED_MODULE(juce, m)
{
    // Register juce_core bindings
    straw::Bindings::registerJuceCoreBindings (m);

    // Register juce_graphics bindings
    straw::Bindings::registerJuceGraphicsBindings (m);

    // Register juce_gui_basics bindings
    straw::Bindings::registerJuceGuiBasicsBindings (m);
}
