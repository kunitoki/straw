/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "straw_Request.h"
#include "../scripting/straw_ScriptEngine.h"
#include "../scripting/straw_ScriptBindings.h"

#include <functional>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <variant>

namespace straw {

//=================================================================================================

/**
 * @brief Create a JSON-style var representing a successful result.
 *
 * This function takes a value of type `juce::var` and wraps it in a JSON-style var to represent a successful result. It can be used to
 * prepare a response to an HTTP request where the operation was successful.
 *
 * @param value The value to be wrapped in the result var.
 *
 * @return A `juce::var` representing the successful result.
 */
juce::var makeResultVar (const juce::var& value);

/**
 * @brief Create a JSON-style var representing an error message.
 *
 * This function takes a message string and creates a JSON-style var to represent an error. It is typically used to prepare an error response
 * for an HTTP request.
 *
 * @param message The error message to be included in the var.
 *
 * @return A `juce::var` representing the error message.
 */
juce::var makeErrorVar (juce::StringRef message);

//=================================================================================================

/**
 * @brief Send an HTTP response using a var.
 *
 * This function sends an HTTP response using a `juce::var` as the response body. It allows you to specify the HTTP status code
 * for the response.
 *
 * @param response The `juce::var` containing the response body data.
 * @param status The HTTP status code to be included in the response.
 * @param connection The `StreamingSocket` used to send the response.
 */
void sendHttpResponse (const juce::var& response, int status, juce::StreamingSocket& connection);

/**
 * @brief Send an HTTP response using a MemoryBlock.
 *
 * This function sends an HTTP response using a `MemoryBlock` as the response body. It allows you to specify the content type and
 * HTTP status code for the response.
 *
 * @param response The `MemoryBlock` containing the response body data.
 * @param contentType The content type of the response (e.g., "application/json").
 * @param status The HTTP status code to be included in the response.
 * @param connection The `StreamingSocket` used to send the response.
 */
void sendHttpResponse (const juce::MemoryBlock& response, juce::StringRef contentType, int status, juce::StreamingSocket& connection);

/**
 * @brief Send an HTTP response containing an image.
 *
 * This function sends an HTTP response containing an image. It allows you to specify the HTTP status code for the response.
 *
 * @param image The `juce::Image` to be included in the response.
 * @param status The HTTP status code to be included in the response.
 * @param connection The `StreamingSocket` used to send the response.
 */
void sendHttpResponse (const juce::Image& image, int status, juce::StreamingSocket& connection);

/**
 * @brief Send an HTTP error message response.
 *
 * This function sends an HTTP response with an error message. It allows you to specify the error message, HTTP status code, and the
 * `StreamingSocket` to send the response.
 *
 * @param message The error message to be included in the response.
 * @param status The HTTP status code to be included in the response.
 * @param connection The `StreamingSocket` used to send the response.
 */
void sendHttpErrorMessage (juce::StringRef message, int status, juce::StreamingSocket& connection);

//=================================================================================================

/**
 * @brief A class for managing an automation server.
 *
 * This class provides functionality for managing an automation server that communicates with clients over a network connection.
 */
class AutomationServer : private juce::Thread
{
public:
    /**
     * @brief Callback type for handling incoming requests.
     *
     * This callback is invoked when a request is received by the automation server. Clients can register their custom endpoint callbacks using this type.
     *
     * @param request The incoming request to be processed.
     */
    using EndpointCallback = std::function<void (Request)>;

    /**
     * @brief Constructor for the AutomationServer class.
     */
    AutomationServer();

    /**
     * @brief Destructor for the AutomationServer class.
     */
    ~AutomationServer() override;

    /**
     * @brief Starts the automation server on the specified port.
     *
     * @param port The port number on which the server should listen. Default is 8001.
     *
     * @return A juce::Result indicating the success or failure of the server start operation.
     */
    [[nodiscard]] juce::Result start (int port = 8001);

    /**
     * @brief Stops the automation server.
     *
     * This function stops the automation server and closes the network connection.
     */
    void stop();

    /**
     * @brief Registers an endpoint with a callback function.
     *
     * Clients can register custom endpoints using this function. When a request is received at the specified path, the associated callback function will be called.
     *
     * @param path The path for the endpoint.
     * @param callback The callback function to handle requests for this endpoint.
     */
    void registerEndpoint (juce::StringRef path, EndpointCallback callback);

    /**
     * @brief Registers default endpoints for common operations.
     *
     * This function registers default endpoints for common automation operations. Clients can use these endpoints to perform standard actions.
     */
    void registerDefaultEndpoints();

    /**
     * @brief Registers a component type and its caster function.
     *
     * This function is used to register custom component types and their corresponding casting functions. This is useful for client-specific component handling.
     *
     * @param className The name of the component class.
     * @param classCaster The casting function for the component class.
     */
    void registerComponentType (juce::StringRef className, ComponentTypeCaster classCaster);

    /**
     * @brief Registers default component types and their casters.
     *
     * This function registers default component types and their associated casting functions. Clients can use these to handle common component types.
     */
    void registerDefaultComponents();
    
    /**
     * @brief Registers custom Python modules for scripting.
     *
     * Clients can register custom Python modules to be available for scripting purposes. Those modules will be implicitly imported by remote scripts.
     *
     * @param modules An initializer list of module names to be registered.
     */
    void registerCustomPythonModules (std::initializer_list<const char*> modules);

private:
    void run() override;

    juce::File getLocalRunFile() const;
    
    void handleConnection (std::shared_ptr<juce::StreamingSocket> connection);
    void handleApplicationJsonRequest (Request request);
    void handlePythonScriptRequest (Request request);

    juce::StreamingSocket socket;
    juce::ThreadPool connectionPool;

    juce::CriticalSection callbacksLock;
    std::unordered_map<juce::String, EndpointCallback> callbacks;
    juce::Array<juce::String> modulesToImport;

    int localPort = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationServer)
    JUCE_DECLARE_WEAK_REFERENCEABLE (AutomationServer)
};

} // namespace straw
