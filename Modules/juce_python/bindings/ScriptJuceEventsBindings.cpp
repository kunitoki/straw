/**
 * juce python - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "ScriptJuceGuiBasicsBindings.h"
#include "ScriptJuceEventsBindings.h"

//#include "../../values/straw_VariantConverter.h"

#include "../pybind11/operators.h"
#include "../pybind11/functional.h"

#include <functional>
#include <string_view>
#include <typeinfo>
#include <tuple>

//=================================================================================================

namespace jucepy::Bindings {

void registerJuceEventsBindings (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    // ============================================================================================ juce::JUCEApplicationBase

    py::class_<JUCEApplicationBase> classJUCEApplicationBase (m, "JUCEApplicationBase");
    classJUCEApplicationBase
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

    // ============================================================================================ juce::ActionListener

    struct PyActionListener : public ActionListener
    {
        void actionListenerCallback(const String& message) override
        {
            PYBIND11_OVERRIDE_PURE(void, ActionListener, actionListenerCallback, message);
        }
    };

    py::class_<ActionListener, PyActionListener> classActionListener (m, "ActionListener");
    classActionListener
        .def (py::init<>())
        .def ("actionListenerCallback", &ActionListener::actionListenerCallback)
    ;

    // ============================================================================================ juce::ActionBroadcaster

    py::class_<ActionBroadcaster> classActionBroadcaster (m, "ActionBroadcaster");
    classActionBroadcaster
        .def (py::init<>())
        .def ("addActionListener", &ActionBroadcaster::addActionListener)
        .def ("removeActionListener", &ActionBroadcaster::removeActionListener)
        .def ("removeAllActionListeners", &ActionBroadcaster::removeAllActionListeners)
        .def ("sendActionMessage", &ActionBroadcaster::sendActionMessage)
    ;

    // ============================================================================================ juce::AsyncUpdater

    struct PyAsyncUpdater : public AsyncUpdater
    {
        void handleAsyncUpdate() override
        {
            PYBIND11_OVERRIDE_PURE(void, AsyncUpdater, handleAsyncUpdate);
        }
    };

    py::class_<AsyncUpdater, PyAsyncUpdater> classAsyncUpdater (m, "AsyncUpdater");
    classAsyncUpdater
        .def (py::init<>())
        .def ("handleAsyncUpdate", &AsyncUpdater::handleAsyncUpdate)
        .def ("triggerAsyncUpdate", &AsyncUpdater::triggerAsyncUpdate)
        .def ("cancelPendingUpdate", &AsyncUpdater::cancelPendingUpdate)
        .def ("handleUpdateNowIfNeeded", &AsyncUpdater::handleUpdateNowIfNeeded)
        .def ("isUpdatePending", &AsyncUpdater::isUpdatePending)
    ;

    // ============================================================================================ juce::MessageManager

    py::class_<MessageManager> classMessageManager (m, "MessageManager");
    classMessageManager
        .def_static ("getInstance", &MessageManager::getInstance)
        .def_static ("getInstanceWithoutCreating", &MessageManager::getInstanceWithoutCreating)
        .def_static ("deleteInstance", &MessageManager::deleteInstance)
        .def ("runDispatchLoop", &MessageManager::runDispatchLoop)
        .def ("stopDispatchLoop", &MessageManager::stopDispatchLoop)
        .def ("hasStopMessageBeenSent", &MessageManager::hasStopMessageBeenSent)
#if JUCE_MODAL_LOOPS_PERMITTED
        .def ("runDispatchLoopUntil", &MessageManager::runDispatchLoopUntil)
#endif
        .def_static ("callAsync", &MessageManager::callAsync) // TODO - test
        .def ("callFunctionOnMessageThread", &MessageManager::callFunctionOnMessageThread)
        .def ("isThisTheMessageThread", &MessageManager::isThisTheMessageThread)
        .def ("setCurrentThreadAsMessageThread", &MessageManager::setCurrentThreadAsMessageThread)
    //.def ("getCurrentMessageThread", &MessageManager::getCurrentMessageThread)
        .def ("currentThreadHasLockedMessageManager", &MessageManager::currentThreadHasLockedMessageManager)
        .def_static ("existsAndIsLockedByCurrentThread", &MessageManager::existsAndIsLockedByCurrentThread)
        .def_static ("existsAndIsCurrentThread", &MessageManager::existsAndIsCurrentThread)
        .def_static ("broadcastMessage", &MessageManager::broadcastMessage)
        .def ("registerBroadcastListener", &MessageManager::registerBroadcastListener)
        .def ("deregisterBroadcastListener", &MessageManager::deregisterBroadcastListener)
        .def ("deliverBroadcastMessage", &MessageManager::deliverBroadcastMessage)
    ;

    // ============================================================================================ juce::MessageManagerLock

    py::class_<MessageManagerLock> classMessageManagerLock (m, "MessageManagerLock");
    classMessageManagerLock
        .def (py::init<>())
        .def ("lockWasGained", &MessageManagerLock::lockWasGained)
    ;

    // ============================================================================================ juce::Timer

    struct PyTimer : public Timer
    {
        void timerCallback() override
        {
            PYBIND11_OVERRIDE_PURE(void, Timer, timerCallback);
        }
    };

    py::class_<Timer, PyTimer> classTimer (m, "Timer");
    classTimer
        .def (py::init<>())
        .def ("timerCallback", &Timer::timerCallback)
        .def ("startTimer", &Timer::startTimer)
        .def ("startTimerHz", &Timer::startTimerHz)
        .def ("stopTimer", &Timer::stopTimer)
        .def ("isTimerRunning", &Timer::isTimerRunning)
        .def ("getTimerInterval", &Timer::getTimerInterval)
        .def_static ("callAfterDelay", &Timer::callAfterDelay)
        .def_static ("callPendingTimersSynchronously", &Timer::callPendingTimersSynchronously)
    ;
}

} // namespace jucepy::Bindings