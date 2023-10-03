/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "straw_ScriptJuceCoreBindings.h"
#include "../straw_ScriptBindings.h"
#include "../straw_ScriptException.h"

#include "../../values/straw_VariantConverter.h"

#include "../pybind11/operators.h"

namespace PYBIND11_NAMESPACE {
namespace detail {

//=================================================================================================

bool type_caster<juce::StringRef>::load (handle src, bool)
{
    PyObject* source = src.ptr();

    auto baseType = Py_TYPE (source)->tp_base;
    if (baseType == &PyUnicode_Type)
    {
        Py_ssize_t size = -1;
        const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (source, &size));
        if (buffer == nullptr)
            return false;

        value = buffer;
        return true;
    }

    return false;
}

handle type_caster<juce::StringRef>::cast (const juce::StringRef& src, return_value_policy policy, handle parent)
{
    juce::ignoreUnused (policy, parent);

    return PyUnicode_FromStringAndSize (static_cast<const char*> (src.text), static_cast<Py_ssize_t> (src.length()));
}

//=================================================================================================

bool type_caster<juce::String>::load (handle src, bool)
{
    PyObject* source = src.ptr();

    auto baseType = Py_TYPE (source)->tp_base;
    if (baseType == &PyUnicode_Type)
    {
        Py_ssize_t size = -1;
        const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (source, &size));
        if (buffer == nullptr)
            return false;

        value = juce::String::fromUTF8 (buffer, static_cast<int> (size));
        return true;
    }

    return false;
}

handle type_caster<juce::String>::cast (const juce::String& src, return_value_policy policy, handle parent)
{
    juce::ignoreUnused (policy, parent);

    return PyUnicode_FromStringAndSize (src.toRawUTF8(), static_cast<Py_ssize_t> (src.getNumBytesAsUTF8()));
}

//=================================================================================================

bool type_caster<juce::Identifier>::load (handle src, bool)
{
    PyObject* source = src.ptr();

    auto baseType = Py_TYPE (source)->tp_base;
    if (baseType == &PyUnicode_Type)
    {
        Py_ssize_t size = -1;
        const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (source, &size));
        if (! buffer)
            return false;

        value = juce::Identifier (juce::String::fromUTF8 (buffer, static_cast<int> (size)));
        return true;
    }

    return false;
}

handle type_caster<juce::Identifier>::cast (const juce::Identifier& src, return_value_policy policy, handle parent)
{
    juce::ignoreUnused (policy, parent);

    return make_caster<juce::String>::cast (src.toString(), policy, parent);
}

//=================================================================================================

bool type_caster<juce::var>::load (handle src, bool)
{
    PyObject* source = src.ptr();

    auto baseType = Py_TYPE (source)->tp_base;
    if (baseType == &PyBool_Type)
        value = PyObject_IsTrue (source) ? true : false;

    else if (baseType == &PyLong_Type)
        value = static_cast<int> (PyLong_AsLong (source));

    else if (baseType == &PyFloat_Type)
        value = PyFloat_AsDouble (source);

    else if (baseType == &PyUnicode_Type)
        value = juce::String::fromUTF8 (PyUnicode_AsUTF8 (source));

    else
        value = juce::var::undefined();

    // TODO - raise

    return !PyErr_Occurred();
}

handle type_caster<juce::var>::cast (const juce::var& src, return_value_policy policy, handle parent)
{
    if (src.isBool())
        return PyBool_FromLong (static_cast<bool> (src));

    else if (src.isInt())
        return PyLong_FromLong (static_cast<int> (src));

    else if (src.isInt64())
        return PyLong_FromLongLong (static_cast<int64_t> (src));

    else if (src.isDouble())
        return PyFloat_FromDouble (static_cast<double> (src));

    else if (src.isString())
        return make_caster<juce::String>::cast (src, policy, parent);

    else if (src.isVoid() || src.isUndefined())
        return Py_None;

    else if (src.isObject())
    {
        auto dynamicObject = src.getDynamicObject();
        if (dynamicObject == nullptr)
            return Py_None;

        object result;

        for (const auto& props : dynamicObject->getProperties())
            result [props.name.toString().toRawUTF8()] = props.value;

        return result;
    }

    else if (src.isArray())
    {
        list list;

        if (auto array = src.getArray())
        {
            for (const auto& value : *array)
                list.append (value);
        }

        return list;
    }

    else if (src.isBinaryData())
    {
        if (auto data = src.getBinaryData())
            return bytes (static_cast<const char*> (data->getData()), static_cast<Py_ssize_t> (data->getSize()));
    }

    else if (src.isMethod())
    {
        return cpp_function ([src]
        {
            juce::var::NativeFunctionArgs args (juce::var(), nullptr, 0);
            return src.getNativeFunction() (args);
        });
    }

    return Py_None;
}

//=================================================================================================

bool type_caster<juce::StringArray>::load (handle src, bool convert)
{
    if (! isinstance<dict> (src))
        return false;

    value.clear();

    auto d = reinterpret_borrow<list> (src);
    for (auto it : d)
    {
        make_caster<juce::String> conv;

        if (! conv.load (it.ptr(), convert))
            return false;

        value.add (cast_op<juce::String&&> (std::move (conv)));
    }

    return true;
}

handle type_caster<juce::StringArray>::cast (const juce::StringArray& src, return_value_policy policy, handle parent)
{
    list l;

    for (const auto& value : src)
    {
        auto item = reinterpret_steal<object> (make_caster<juce::String>::cast (value, policy, parent));

        if (! item)
            return handle();

        l.append (std::move (item));
    }

    return l.release();
}

//=================================================================================================

bool type_caster<juce::NamedValueSet>::load (handle src, bool convert)
{
    if (! isinstance<dict> (src))
        return false;

    value.clear();

    auto d = reinterpret_borrow<dict> (src);
    for (auto it : d)
    {
        make_caster<juce::Identifier> kconv;
        make_caster<juce::var> vconv;

        if (! kconv.load (it.first.ptr(), convert) || ! vconv.load (it.second.ptr(), convert))
            return false;

        value.set (cast_op<juce::Identifier&&> (std::move (kconv)), cast_op<juce::var&&> (std::move (vconv)));
    }

    return true;
}

handle type_caster<juce::NamedValueSet>::cast (const juce::NamedValueSet& src, return_value_policy policy, handle parent)
{
    dict d;

    for (const auto& kv : src)
    {
        auto key = reinterpret_steal<object> (make_caster<juce::Identifier>::cast (kv.name, policy, parent));
        auto value = reinterpret_steal<object> (make_caster<juce::var>::cast (kv.value, policy, parent));

        if (! key || ! value)
            return handle();

        d [std::move (key)] = std::move (value);
    }

    return d.release();
}

}} // namespace PYBIND11_NAMESPACE::detail

//=================================================================================================

namespace straw::Bindings {

void registerJuceCoreBindings ([[maybe_unused]] pybind11::module_& m)
{
    using namespace juce;
    namespace py = pybind11;

    // ============================================================================================ juce::MemoryBlock
    
    py::class_<MemoryBlock> classMemoryBlock (m, "MemoryBlock");
    classMemoryBlock
        .def (py::init<>())
        .def (py::init<const size_t, bool>())
        .def (py::init<const MemoryBlock&>())
        //.def (py::init<const void*, size_t>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("matches", &MemoryBlock::matches)
        .def ("getData", py::overload_cast<>(&MemoryBlock::getData))
        .def ("getData", py::overload_cast<>(&MemoryBlock::getData, py::const_))
        .def ("isEmpty", &MemoryBlock::isEmpty)
        .def ("getSize", &MemoryBlock::getSize)
        .def ("setSize", &MemoryBlock::setSize)
        .def ("ensureSize", &MemoryBlock::ensureSize)
        .def ("reset", &MemoryBlock::reset)
        .def ("fillWith", &MemoryBlock::fillWith)
        .def ("append", &MemoryBlock::append)
        .def ("replaceAll", &MemoryBlock::replaceAll)
        .def ("insert", &MemoryBlock::insert)
        .def ("removeSection", &MemoryBlock::removeSection)
        .def ("copyFrom", &MemoryBlock::copyFrom)
        .def ("copyTo", &MemoryBlock::copyTo)
        .def ("swapWith", &MemoryBlock::swapWith)
        .def ("toString", &MemoryBlock::toString)
        .def ("loadFromHexString", &MemoryBlock::loadFromHexString)
        .def ("setBitRange", &MemoryBlock::setBitRange)
        .def ("getBitRange", &MemoryBlock::getBitRange)
        .def ("toBase64Encoding", &MemoryBlock::toBase64Encoding)
        .def ("fromBase64Encoding", &MemoryBlock::fromBase64Encoding)
    ;

    // ============================================================================================ juce::CriticalSection

    py::class_<CriticalSection> classCriticalSection (m, "CriticalSection");
    classCriticalSection
        .def (py::init<>())
        .def ("enter", &CriticalSection::enter)
        .def ("tryEnter", &CriticalSection::tryEnter)
        .def ("exit", &CriticalSection::exit)
    ;

    // ============================================================================================ juce::SpinLock

    py::class_<SpinLock> classSpinLock (m, "SpinLock");
    classSpinLock
        .def (py::init<>())
        .def ("enter", &SpinLock::enter)
        .def ("tryEnter", &SpinLock::tryEnter)
        .def ("exit", &SpinLock::exit)
    ;

    // ============================================================================================ juce::WaitableEvent

    py::class_<WaitableEvent> classWaitableEvent (m, "WaitableEvent");
    classWaitableEvent
        .def (py::init<>())
        .def (py::init<bool>())
        .def ("wait", &WaitableEvent::wait)
        .def ("signal", &WaitableEvent::signal)
        .def ("reset", &WaitableEvent::reset)
    ;

    // ============================================================================================ juce::ReadWriteLock

    py::class_<ReadWriteLock> classReadWriteLock (m, "ReadWriteLock");
    classReadWriteLock
        .def (py::init<>())
        .def ("enterRead", &ReadWriteLock::enterRead)
        .def ("tryEnterRead", &ReadWriteLock::tryEnterRead)
        .def ("exitRead", &ReadWriteLock::exitRead)
        .def ("enterWrite", &ReadWriteLock::enterWrite)
        .def ("tryEnterWrite", &ReadWriteLock::tryEnterWrite)
        .def ("exitWrite", &ReadWriteLock::exitWrite)
    ;

    // ============================================================================================ juce::InterProcessLock

    py::class_<InterProcessLock> classInterProcessLock (m, "InterProcessLock");
    classInterProcessLock
        .def (py::init<const String&>())
        .def ("enter", &InterProcessLock::enter)
        .def ("exit", &InterProcessLock::exit)
    ;

    // ============================================================================================ juce::HighResolutionTimer

    struct PyHighResolutionTimer : public HighResolutionTimer
    {
        void hiResTimerCallback() override
        {
            PYBIND11_OVERRIDE_PURE(void, HighResolutionTimer, hiResTimerCallback);
        }
    };

    py::class_<HighResolutionTimer, PyHighResolutionTimer> classHighResolutionTimer (m, "HighResolutionTimer");
    classHighResolutionTimer
        .def (py::init<>())
        .def ("hiResTimerCallback", &HighResolutionTimer::hiResTimerCallback)
        .def ("startTimer", &HighResolutionTimer::startTimer)
        .def ("stopTimer", &HighResolutionTimer::stopTimer)
        .def ("isTimerRunning", &HighResolutionTimer::isTimerRunning)
        .def ("getTimerInterval", &HighResolutionTimer::getTimerInterval)
    ;

    // ============================================================================================ juce::ChildProcess

    py::class_<ChildProcess> classChildProcess (m, "ChildProcess");
    classChildProcess
        .def (py::init<>())
        .def ("start", py::overload_cast<const String &, int>(&ChildProcess::start))
        .def ("start", py::overload_cast<const StringArray &, int>(&ChildProcess::start))
        .def ("isRunning", &ChildProcess::isRunning)
        .def ("readProcessOutput", &ChildProcess::readProcessOutput)
        .def ("readAllProcessOutput", &ChildProcess::readAllProcessOutput)
        .def ("waitForProcessToFinish", &ChildProcess::waitForProcessToFinish)
        .def ("getExitCode", &ChildProcess::getExitCode)
        .def ("kill", &ChildProcess::kill)
    ; 

    // ============================================================================================ juce::File

    py::class_<File> classFile (m, "File");
    classFile
        .def (py::init<>())
        .def (py::init<const String&>())
        .def (py::init<const File&>())
        .def ("exists", &File::exists)
        .def ("existsAsFile", &File::existsAsFile)
        .def ("isDirectory", &File::isDirectory)
        .def ("isRoot", &File::isRoot)
        .def ("getSize", &File::getSize)
        .def_static ("descriptionOfSizeInBytes", &File::descriptionOfSizeInBytes)
        .def ("getFullPathName", &File::getFullPathName)
        .def ("getFileName", &File::getFileName)
        .def ("getRelativePathFrom", &File::getRelativePathFrom)
        .def ("getFileExtension", &File::getFileExtension)
        .def ("hasFileExtension", &File::hasFileExtension)
        .def ("withFileExtension", &File::withFileExtension)
        .def ("getFileNameWithoutExtension", &File::getFileNameWithoutExtension)
        .def ("hashCode", &File::hashCode)
        .def ("hashCode64", &File::hashCode64)
        .def ("getChildFile", &File::getChildFile)
        .def ("getSiblingFile", &File::getSiblingFile)
        .def ("getParentDirectory", &File::getParentDirectory)
        .def ("isAChildOf", &File::isAChildOf)
        .def ("getNonexistentChildFile", &File::getNonexistentChildFile)
        .def ("getNonexistentSibling", &File::getNonexistentSibling)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self < py::self)
        .def (py::self > py::self)
        .def ("hasWriteAccess", &File::hasWriteAccess)
        .def ("hasReadAccess", &File::hasReadAccess)
        .def ("setReadOnly", &File::setReadOnly)
        .def ("setExecutePermission", &File::setExecutePermission)
        .def ("isHidden", &File::isHidden)
        .def ("getFileIdentifier", &File::getFileIdentifier)
        .def ("getLastModificationTime", &File::getLastModificationTime)
        .def ("getLastAccessTime", &File::getLastAccessTime)
        .def ("getCreationTime", &File::getCreationTime)
        .def ("setLastModificationTime", &File::setLastModificationTime)
        .def ("setLastAccessTime", &File::setLastAccessTime)
        .def ("setCreationTime", &File::setCreationTime)
        .def ("getVersion", &File::getVersion)
        .def ("create", &File::create)
        .def ("createDirectory", &File::createDirectory)
        .def ("deleteFile", &File::deleteFile)
        .def ("deleteRecursively", &File::deleteRecursively)
        .def ("moveToTrash", &File::moveToTrash)
        .def ("moveFileTo", &File::moveFileTo)
        .def ("copyFileTo", &File::copyFileTo)
        .def ("replaceFileIn", &File::replaceFileIn)
        .def ("copyDirectoryTo", &File::copyDirectoryTo)
        .def ("findChildFiles", py::overload_cast<int, bool, const String &, File::FollowSymlinks>(&File::findChildFiles, py::const_))
        //.def ("findChildFiles", py::overload_cast<int &, int, bool, const String &, File::FollowSymlinks>(&File::findChildFiles, py::const_))
        .def ("getNumberOfChildFiles", &File::getNumberOfChildFiles)
        .def ("containsSubDirectories", &File::containsSubDirectories)
        .def ("createInputStream", &File::createInputStream)
        .def ("createOutputStream", &File::createOutputStream)
        .def ("loadFileAsData", &File::loadFileAsData)
        .def ("loadFileAsString", &File::loadFileAsString)
        .def ("readLines", &File::readLines)
        .def ("appendData", &File::appendData)
        .def ("replaceWithData", &File::replaceWithData)
        .def ("appendText", &File::appendText)
        .def ("replaceWithText", &File::replaceWithText)
        .def ("hasIdenticalContentTo", &File::hasIdenticalContentTo)
        .def_static ("findFileSystemRoots", &File::findFileSystemRoots)
        .def ("getVolumeLabel", &File::getVolumeLabel)
        .def ("getVolumeSerialNumber", &File::getVolumeSerialNumber)
        .def ("getBytesFreeOnVolume", &File::getBytesFreeOnVolume)
        .def ("getVolumeTotalSize", &File::getVolumeTotalSize)
        .def ("isOnCDRomDrive", &File::isOnCDRomDrive)
        .def ("isOnHardDisk", &File::isOnHardDisk)
        .def ("isOnRemovableDrive", &File::isOnRemovableDrive)
        .def ("startAsProcess", &File::startAsProcess)
        .def ("revealToUser", &File::revealToUser)
        .def_static ("getSpecialLocation", &File::getSpecialLocation)
        .def_static ("createTempFile", &File::createTempFile)
        .def_static ("getCurrentWorkingDirectory", &File::getCurrentWorkingDirectory)
        .def ("setAsCurrentWorkingDirectory", &File::setAsCurrentWorkingDirectory)
        .def_static ("getSeparatorChar", &File::getSeparatorChar)
        .def_static ("getSeparatorString", &File::getSeparatorString)
        .def_static ("createLegalFileName", &File::createLegalFileName)
        .def_static ("createLegalPathName", &File::createLegalPathName)
        .def_static ("areFileNamesCaseSensitive", &File::areFileNamesCaseSensitive)
        .def_static ("isAbsolutePath", &File::isAbsolutePath)
        .def_static ("createFileWithoutCheckingPath", &File::createFileWithoutCheckingPath)
        .def_static ("addTrailingSeparator", &File::addTrailingSeparator)
        .def ("createSymbolicLink", py::overload_cast<const File &, bool>(&File::createSymbolicLink, py::const_))
        .def_static ("createSymbolicLink", static_cast<bool (*)(const File &, const String &, bool)>(&File::createSymbolicLink))
        .def ("isSymbolicLink", &File::isSymbolicLink)
        .def ("getLinkedTarget", &File::getLinkedTarget)
        .def ("getNativeLinkedTarget", &File::getNativeLinkedTarget)
        // .def ("getMacOSType", &File::getMacOSType)
        .def ("isBundle", &File::isBundle)
        .def ("addToDock", &File::addToDock)
        .def_static ("getContainerForSecurityApplicationGroupIdentifier", &File::getContainerForSecurityApplicationGroupIdentifier)
    ;

    py::enum_<File::SpecialLocationType> (classFile, "SpecialLocationType")
        .value("commonApplicationDataDirectory", File::SpecialLocationType::commonApplicationDataDirectory)
        .value("commonDocumentsDirectory", File::SpecialLocationType::commonDocumentsDirectory)
        .value("currentApplicationFile", File::SpecialLocationType::currentApplicationFile)
        .value("currentExecutableFile", File::SpecialLocationType::currentExecutableFile)
        .value("globalApplicationsDirectory", File::SpecialLocationType::globalApplicationsDirectory)
        .value("hostApplicationPath", File::SpecialLocationType::hostApplicationPath)
        .value("invokedExecutableFile", File::SpecialLocationType::invokedExecutableFile)
        .value("tempDirectory", File::SpecialLocationType::tempDirectory)
        .value("userApplicationDataDirectory", File::SpecialLocationType::userApplicationDataDirectory)
        .value("userDesktopDirectory", File::SpecialLocationType::userDesktopDirectory)
        .value("userDocumentsDirectory", File::SpecialLocationType::userDocumentsDirectory)
        .value("userHomeDirectory", File::SpecialLocationType::userHomeDirectory)
        .value("userMoviesDirectory", File::SpecialLocationType::userMoviesDirectory)
        .value("userMusicDirectory", File::SpecialLocationType::userMusicDirectory)
        .value("userPicturesDirectory", File::SpecialLocationType::userPicturesDirectory)
        .export_values();

    py::enum_<File::FollowSymlinks> (classFile, "FollowSymlinks")
        .value("no", File::FollowSymlinks::no)
        .value("noCycles", File::FollowSymlinks::noCycles)
        .value("yes", File::FollowSymlinks::yes)
        .export_values();

    // ============================================================================================ juce::URL
    
    py::class_<URL> classURL (m, "URL");
    classURL
        .def (py::init<>())
        .def (py::init<const String&>())
        .def (py::init<File>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("toString", &URL::toString)
        .def ("isEmpty", &URL::isEmpty)
        .def ("isWellFormed", &URL::isWellFormed)
        .def ("getDomain", &URL::getDomain)
        .def ("getSubPath", &URL::getSubPath)
        .def ("getQueryString", &URL::getQueryString)
        .def ("getAnchorString", &URL::getAnchorString)
        .def ("getScheme", &URL::getScheme)
        .def ("isLocalFile", &URL::isLocalFile)
        .def ("getLocalFile", &URL::getLocalFile)
        .def ("getFileName", &URL::getFileName)
        .def ("getPort", &URL::getPort)
        .def ("withNewDomainAndPath", &URL::withNewDomainAndPath)
        .def ("withNewSubPath", &URL::withNewSubPath)
        .def ("getParentURL", &URL::getParentURL)
        .def ("getChildURL", &URL::getChildURL)
        .def ("withParameter", &URL::withParameter)
        .def ("withParameters", &URL::withParameters)
        .def ("withAnchor", &URL::withAnchor)
        .def ("withFileToUpload", &URL::withFileToUpload)
        .def ("withDataToUpload", &URL::withDataToUpload)
        .def ("getParameterNames", &URL::getParameterNames)
        .def ("getParameterValues", &URL::getParameterValues)
        .def ("withPOSTData", py::overload_cast<const String &>(&URL::withPOSTData, py::const_))
        .def ("withPOSTData", py::overload_cast<const MemoryBlock &>(&URL::withPOSTData, py::const_))
        .def ("getPostData", &URL::getPostData)
        .def ("getPostDataAsMemoryBlock", &URL::getPostDataAsMemoryBlock)
        .def ("launchInDefaultBrowser", &URL::launchInDefaultBrowser)
        .def_static ("isProbablyAWebsiteURL", &URL::isProbablyAWebsiteURL)
        .def_static ("isProbablyAnEmailAddress", &URL::isProbablyAnEmailAddress)
        .def ("createInputStream", py::overload_cast<const URL::InputStreamOptions &>(&URL::createInputStream, py::const_))
        .def ("createOutputStream", &URL::createOutputStream)
        .def ("downloadToFile", py::overload_cast<const File &, const URL::DownloadTaskOptions &>(&URL::downloadToFile))
        .def ("readEntireBinaryStream", &URL::readEntireBinaryStream)
        .def ("readEntireTextStream", &URL::readEntireTextStream)
        .def ("readEntireXmlStream", &URL::readEntireXmlStream)
        .def_static ("addEscapeChars", &URL::addEscapeChars)
        .def_static ("removeEscapeChars", &URL::removeEscapeChars)
        .def_static ("createWithoutParsing", &URL::createWithoutParsing)
    ;

    py::class_<URL::InputStreamOptions> classURLInputStreamOptions (classURL, "InputStreamOptions");
    classURLInputStreamOptions
        .def ("withProgressCallback", &URL::InputStreamOptions::withProgressCallback)
        .def ("withExtraHeaders", &URL::InputStreamOptions::withExtraHeaders)
        .def ("withConnectionTimeoutMs", &URL::InputStreamOptions::withConnectionTimeoutMs)
        .def ("withResponseHeaders", &URL::InputStreamOptions::withResponseHeaders)
        .def ("withStatusCode", &URL::InputStreamOptions::withStatusCode)
        .def ("withNumRedirectsToFollow", &URL::InputStreamOptions::withNumRedirectsToFollow)
        .def ("withHttpRequestCmd", &URL::InputStreamOptions::withHttpRequestCmd)
        .def ("getParameterHandling", &URL::InputStreamOptions::getParameterHandling)
        .def ("getProgressCallback", &URL::InputStreamOptions::getProgressCallback)
        .def ("getExtraHeaders", &URL::InputStreamOptions::getExtraHeaders)
        .def ("getConnectionTimeoutMs", &URL::InputStreamOptions::getConnectionTimeoutMs)
        .def ("getResponseHeaders", &URL::InputStreamOptions::getResponseHeaders)
        .def ("getStatusCode", &URL::InputStreamOptions::getStatusCode)
        .def ("getNumRedirectsToFollow", &URL::InputStreamOptions::getNumRedirectsToFollow)
        .def ("getHttpRequestCmd", &URL::InputStreamOptions::getHttpRequestCmd)
    ;

    py::class_<URL::DownloadTask> classURLDownloadTask (classURL, "DownloadTask");
    classURLDownloadTask
        .def ("getTotalLength", &URL::DownloadTask::getTotalLength)
        .def ("getLengthDownloaded", &URL::DownloadTask::getLengthDownloaded)
        .def ("isFinished", &URL::DownloadTask::isFinished)
        .def ("statusCode", &URL::DownloadTask::statusCode)
        .def ("hadError", &URL::DownloadTask::hadError)
        .def ("getTargetLocation", &URL::DownloadTask::getTargetLocation)
    ;
    
    struct PyURLDownloadTaskListener : public URL::DownloadTaskListener
    {
        void finished (URL::DownloadTask* task, bool success) override
        {
            PYBIND11_OVERRIDE_PURE(void, URL::DownloadTaskListener, finished, task, success);
        }
        
        void progress (URL::DownloadTask* task, int64 bytesDownloaded, int64 totalLength) override
        {
            PYBIND11_OVERRIDE_PURE(void, URL::DownloadTaskListener, progress, task, bytesDownloaded, totalLength);
        }
    };
    
    py::class_<URL::DownloadTaskListener, PyURLDownloadTaskListener> classURLDownloadTaskListener (classURL, "DownloadTaskListener");
    classURLDownloadTaskListener
        .def ("finished", &URL::DownloadTaskListener::finished)
        .def ("progress", &URL::DownloadTaskListener::progress)
    ;

    py::class_<URL::DownloadTaskOptions> classURLDownloadTaskOptions (classURL, "DownloadTaskOptions");
    classURLDownloadTaskOptions
        .def ("withExtraHeaders", &URL::DownloadTaskOptions::withExtraHeaders)
        .def ("withSharedContainer", &URL::DownloadTaskOptions::withSharedContainer)
        .def ("withListener", &URL::DownloadTaskOptions::withListener)
        .def ("withUsePost", &URL::DownloadTaskOptions::withUsePost)
        .def_property ("extraHeaders",
                       [](const URL::DownloadTaskOptions& self) { return self.extraHeaders; },
                       [](URL::DownloadTaskOptions& self, const String& v) { self.extraHeaders = v; })
        .def_property ("sharedContainer",
                       [](const URL::DownloadTaskOptions& self) { return self.sharedContainer; },
                       [](URL::DownloadTaskOptions& self, const String& v) { self.sharedContainer = v; })
        .def_property ("listener",
                       [](const URL::DownloadTaskOptions& self) { return self.listener; },
                       [](URL::DownloadTaskOptions& self, URL::DownloadTaskListener* v) { self.listener = v; })
        .def_property ("usePost",
                       [](const URL::DownloadTaskOptions& self) { return self.usePost; },
                       [](URL::DownloadTaskOptions& self, bool v) { self.usePost = v; })
    ;
}

} // namespace straw::Bindings
