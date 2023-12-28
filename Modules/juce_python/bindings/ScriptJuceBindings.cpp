/**
 * juce python - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "ScriptJuceCoreBindings.h"
#include "ScriptJuceEventsBindings.h"
#include "ScriptJuceGraphicsBindings.h"
#include "ScriptJuceGuiBasicsBindings.h"

//=================================================================================================

PYBIND11_EMBEDDED_MODULE(juce, m)
{
    // Register juce_core bindings
    jucepy::Bindings::registerJuceCoreBindings (m);

    // Register juce_events bindings
    jucepy::Bindings::registerJuceEventsBindings (m);

    // Register juce_graphics bindings
    jucepy::Bindings::registerJuceGraphicsBindings (m);

    // Register juce_gui_basics bindings
    jucepy::Bindings::registerJuceGuiBasicsBindings (m);
}
