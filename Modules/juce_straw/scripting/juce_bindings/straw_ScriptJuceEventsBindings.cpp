/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "straw_ScriptJuceGuiBasicsBindings.h"
#include "straw_ScriptJuceEventsBindings.h"
#include "../straw_ScriptBindings.h"
#include "../straw_ScriptException.h"

#include "../../values/straw_VariantConverter.h"
#include "../../helpers/straw_ComponentHelpers.h"

#include "../pybind11/operators.h"

#include <functional>
#include <string_view>
#include <typeinfo>
#include <tuple>

//=================================================================================================

namespace straw::Bindings {

void registerJuceEventsBindings (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    // ============================================================================================ juce::JUCEApplicationBase

    py::class_<JUCEApplicationBase> (m, "JUCEApplicationBase")
        .def ("getApplicationName", &JUCEApplicationBase::getApplicationName)
        .def ("getApplicationVersion", &JUCEApplicationBase::getApplicationVersion)
        .def ("moreThanOneInstanceAllowed", &JUCEApplicationBase::moreThanOneInstanceAllowed)
        .def_static ("quit", &JUCEApplicationBase::quit)
        .def_static ("getCommandLineParameterArray", &JUCEApplicationBase::getCommandLineParameterArray)
        .def_static ("getCommandLineParameters", &JUCEApplicationBase::getCommandLineParameters)
        .def ("setApplicationReturnValue", &JUCEApplicationBase::setApplicationReturnValue)
        .def ("getApplicationReturnValue", &JUCEApplicationBase::getApplicationReturnValue)
        .def_static ("isStandaloneApp", &JUCEApplicationBase::isStandaloneApp)
        .def ("isInitialising", &JUCEApplicationBase::isInitialising)
    ;
}

} // namespace straw::Bindings
