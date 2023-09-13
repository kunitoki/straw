/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "straw_AutomationServer.h"

#include "../scripting/straw_ScriptEngine.h"
#include "../scripting/straw_ScriptBindings.h"
#include "../endpoints/straw_ComponentEndpoints.h"
#include "../helpers/straw_ComponentHelpers.h"

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>

namespace straw {
namespace {

//=================================================================================================

bool startsWithHttpVerb (const juce::String& httpLine)
{
    return httpLine.startsWith ("GET")
        || httpLine.startsWith ("PUT")
        || httpLine.startsWith ("POST");
}

//=================================================================================================

juce::String makeHttpStatusCode (int statusCode)
{
    static const std::unordered_map<int, juce::String> httpStatusCodes
    {
        { 100, "100 Continue" },
        { 200, "200 OK" },
        { 404, "404 Not Found" },
        { 500, "500 Internal Server Error" }
    };

    auto it = httpStatusCodes.find (statusCode);
    if (it == httpStatusCodes.end())
        return "500 Internal Server Error";

    return it->second;
}

//=================================================================================================

juce::MemoryBlock makeHttpResponse (const juce::String& payload, const juce::String& contentType, int status = 200)
{
    juce::String response;
    
    response
    << "HTTP/1.1 " << makeHttpStatusCode (status) << "\n"
    << "Server: Squeeze/0.0.1" << "\n"
    << "Content-Type: " << contentType << "\n"
    << "Content-Length: " << payload.getNumBytesAsUTF8() << "\n"
    << "Connection: Closed" << "\n\n";
    
    juce::MemoryBlock mb;
    mb.append (response.toRawUTF8(), response.getNumBytesAsUTF8());
    mb.append (payload.toRawUTF8(), payload.getNumBytesAsUTF8());
    return mb;
}

juce::MemoryBlock makeHttpResponse (const juce::MemoryBlock& payload, const juce::String& contentType, int status = 200)
{
    juce::String response;
    
    response
    << "HTTP/1.1 " << makeHttpStatusCode (status) << "\n"
    << "Server: Squeeze/0.0.1" << "\n"
    << "Content-Type: " << contentType << "\n"
    << "Content-Length: " << payload.getSize() << "\n"
    << "Connection: Closed" << "\n\n";
    
    juce::MemoryBlock mb;
    mb.append (response.toRawUTF8(), response.getNumBytesAsUTF8());
    mb.append (payload.getData(), payload.getSize());
    return mb;
}

//=================================================================================================

juce::MemoryBlock readHttpPayload (juce::StreamingSocket& connection)
{
    juce::MemoryBlock payload;
    juce::uint8 data[1024] = { 0 };

    while (true)
    {
        auto numBytesRead = connection.read (data, juce::numElementsInArray (data), false);
        if (numBytesRead <= 0)
            break;

        payload.append (data, static_cast<size_t> (numBytesRead));
    }

    return payload;
}

//=================================================================================================

Request parseHttpPayload (const juce::String& payload)
{
    Request request;

    auto requestStrings = juce::StringArray::fromLines (payload);
    for (int i = 0; i < requestStrings.size(); ++i)
    {
        const auto& requestString = requestStrings [i];
        //juce::Logger::writeToLog (requestString);

        if (startsWithHttpVerb (requestString))
        {
            auto pathParts = juce::StringArray::fromTokens (requestString, false);
            if (pathParts.size() >= 1)
                request.verb = pathParts [0];
            if (pathParts.size() >= 2)
                request.path = pathParts [1];
        }
        else if (requestString.startsWith ("Content-Type: "))
        {
            request.contentType = requestString
                .fromFirstOccurrenceOf ("Content-Type: ", false, true)
                .upToFirstOccurrenceOf (";", false, false);
        }
        else if (requestString.startsWith ("Content-Length: "))
        {
            request.contentLength = requestString.fromFirstOccurrenceOf ("Content-Length: ", false, true).getIntValue();
        }
        else if (requestString.isEmpty())
        {
            auto remainingPayload = requestStrings;
            remainingPayload.removeRange (0, i + 1);

            request.contentData = remainingPayload.joinIntoString ("\n");

            break;
        }
    }

    return request;
}

} // namespace

//=================================================================================================

juce::var makeResultVar(const juce::var& value)
{
    juce::DynamicObject::Ptr object = new juce::DynamicObject;
    object->setProperty("result", value);
    return object.get();
}

juce::var makeErrorVar(juce::StringRef message)
{
    juce::DynamicObject::Ptr object = new juce::DynamicObject;
    object->setProperty("error", juce::String(message));
    return object.get();
}

//=================================================================================================

void sendHttpResponse (const juce::MemoryBlock& response, juce::StringRef contentType, int status, juce::StreamingSocket& connection)
{
    auto responseMessage = makeHttpResponse (response, contentType, status);
    connection.write (responseMessage.getData(), static_cast<int> (responseMessage.getSize()));
}

void sendHttpResponse (const juce::Image& image, int status, juce::StreamingSocket& connection)
{
    juce::MemoryBlock mb;
    juce::MemoryOutputStream mos (mb, false);
       
    juce::PNGImageFormat pngFormat;
    if (pngFormat.writeImageToStream(image, mos))
        sendHttpResponse (mb, "image/png", status, connection);
    else
        sendHttpErrorMessage ("Unable to send image", status, connection);
}

void sendHttpResponse (const juce::var& response, int status, juce::StreamingSocket& connection)
{
    auto resultJson = juce::JSON::toString (response);
    juce::Logger::writeToLog (resultJson);

    auto responseMessage = makeHttpResponse (resultJson, "plain/text", status);
    connection.write (responseMessage.getData(), static_cast<int> (responseMessage.getSize()));
}

void sendHttpErrorMessage (juce::StringRef message, int status, juce::StreamingSocket& connection)
{
    sendHttpResponse (makeErrorVar (message), status, connection);
}

//=================================================================================================

AutomationServer::AutomationServer()
    : juce::Thread ("Squeeze Server Thread")
    , connectionPool (juce::ThreadPoolOptions().withThreadName ("Squeeze Requests Thread"))
{
}

AutomationServer::~AutomationServer()
{
    Bindings::clearComponentTypes();

    stopThread (10000);
}

//=================================================================================================

juce::Result AutomationServer::start (int port)
{
    if (socket.isConnected())
        return juce::Result::fail ("Unable to listen, server already listening");

    localPort = -1;

    if (! socket.createListener (port))
        return juce::Result::fail ("Unable to listen to port");

    if (! startThread())
        return juce::Result::fail ("Unable to start thread");

    localPort = port;

    getLocalRunFile().replaceWithText (juce::String (localPort));

    return juce::Result::ok();
}

void AutomationServer::stop()
{
    if (! socket.isConnected() || localPort < 0)
        return;

    signalThreadShouldExit();
    connectionPool.removeAllJobs (true, 10000);

    socket.close();
    
    getLocalRunFile().deleteFile();
}

//=================================================================================================

void AutomationServer::run()
{
    while (! threadShouldExit())
    {
        auto connection = socket.waitForNextConnection();
        if (! connection)
            continue;

        auto pooledConnection = std::shared_ptr<juce::StreamingSocket> (connection, [](auto* conn)
        {
            conn->close();
            delete conn;
        });

        handleConnection (std::move (pooledConnection));
    }
}

//=================================================================================================

void AutomationServer::handleConnection (std::shared_ptr<juce::StreamingSocket> connection)
{
    auto connectionStatus = connection->waitUntilReady (true, 1000);
    if (connectionStatus == -1)
    {
        sendHttpErrorMessage ("failed syncing with connection reading", 500, *connection);
        return;
    }

    juce::MemoryBlock payload = readHttpPayload (*connection);
    if (payload.isEmpty())
    {
        sendHttpErrorMessage ("invalid processing of empty payload", 500, *connection);
        return;
    }

    juce::Logger::writeToLog (payload.toString());

    Request request = parseHttpPayload (payload.toString());
    request.connection = connection;

    if (request.contentLength == 0 || request.contentData.length() != request.contentLength)
    {
        sendHttpErrorMessage ("invalid content length", 500, *connection);
        return;
    }

    if (request.contentType == "application/json")
        handleApplicationJsonRequest (std::move (request));
    
    if (request.contentType == "text/x-python")
        handlePythonScriptRequest (std::move (request));
}

//=================================================================================================

void AutomationServer::handleApplicationJsonRequest (Request request)
{
    if (request.path.isEmpty())
    {
        sendHttpErrorMessage ("failed parsing path", 500, *request.connection);
        return;
    }

    auto result = juce::JSON::parse (request.contentData, request.data);
    if (result.failed())
    {
        sendHttpErrorMessage ("failed parsing json", 500, *request.connection);
        return;
    }

    {
        auto lock = juce::CriticalSection::ScopedLockType(callbacksLock);

        auto it = callbacks.find (request.path);
        if (it == callbacks.end())
        {
            sendHttpErrorMessage ("path not found", 404, *request.connection);
            return;
        }

        connectionPool.addJob ([callback = it->second, request = std::move (request)]
        {
            callback (std::move (request));
        });
    }
}

//=================================================================================================

void AutomationServer::handlePythonScriptRequest (Request request)
{
    juce::MessageManager::callAsync([this, request = std::move (request)]
    {
        ScriptEngine engine ([this]
        {
            auto lock = juce::CriticalSection::ScopedLockType(callbacksLock);
            return modulesToImport;
        }());

        auto result = engine.runScript (request.contentData);
        
        connectionPool.addJob ([result = std::move (result), request = std::move (request)]
        {
            if (result.failed())
            {
                sendHttpErrorMessage (result.getErrorMessage(), 500, *request.connection);
            }
            else
            {
                juce::var response = makeResultVar (true);
                sendHttpResponse (response, 200, *request.connection);
            }
        });
    });
}

//=================================================================================================

void AutomationServer::registerEndpoint (juce::StringRef path, EndpointCallback callback)
{
    auto lock = juce::CriticalSection::ScopedLockType(callbacksLock);

    callbacks [path] = std::move (callback);
}

void AutomationServer::registerDefaultEndpoints()
{
    registerEndpoint ("/straw/component/exists", &Endpoints::componentExists);
    registerEndpoint ("/straw/component/visible", &Endpoints::componentVisible);
    registerEndpoint ("/straw/component/info", &Endpoints::componentInfo);
    registerEndpoint ("/straw/component/click", &Endpoints::componentClick);
    registerEndpoint ("/straw/component/render", &Endpoints::componentRender);
}

//=================================================================================================

void AutomationServer::registerComponentType (juce::StringRef className, ComponentTypeCaster classCaster)
{
    Bindings::registerComponentType (className, std::move (classCaster));
}

void AutomationServer::registerDefaultComponents()
{
    // Buttons
    Bindings::registerComponentType ("juce::Button", &ComponentType<juce::Button>);
    Bindings::registerComponentType ("juce::ArrowButton", &ComponentType<juce::ArrowButton>);
    Bindings::registerComponentType ("juce::DrawableButton", &ComponentType<juce::DrawableButton>);
    Bindings::registerComponentType ("juce::HyperlinkButton", &ComponentType<juce::HyperlinkButton>);
    Bindings::registerComponentType ("juce::ImageButton", &ComponentType<juce::ImageButton>);
    Bindings::registerComponentType ("juce::ShapeButton", &ComponentType<juce::ShapeButton>);
    Bindings::registerComponentType ("juce::TextButton", &ComponentType<juce::TextButton>);
    Bindings::registerComponentType ("juce::ToggleButton", &ComponentType<juce::ToggleButton>);
    
    // Widgets
    Bindings::registerComponentType ("juce::Slider", &ComponentType<juce::Slider>);
}

//=================================================================================================

void AutomationServer::registerCustomPythonModules (std::initializer_list<const char*> modules)
{
    auto lock = juce::CriticalSection::ScopedLockType(callbacksLock);

    for (const auto& m : modules)
        modulesToImport.add (m);
}

//=================================================================================================

juce::File AutomationServer::getLocalRunFile() const
{
    juce::File applicationFile = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
    return applicationFile.getParentDirectory().getChildFile("straw.run");
}

} // namespace straw
