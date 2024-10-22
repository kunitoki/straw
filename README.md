![Straw Banner](https://github.com/kunitoki/straw/blob/main/banner.png?raw=true)

# straw
Straw is a juce automation framework for integration testing.

## Download and install the python framework

Go to https://www.python.org/downloads/ and download a python install. The version *3.11 is reccommended*. After installing it, make sure you write down the location of the install. For example on macOS, typically it is installed in `/Library/Frameworks/Python.framework/Versions/3.11` but you could use the `/Library/Frameworks/Python.framework/Versions/Current` symbolic link for simplicity.

## Setup the dependency using cmake

In order to configure straw in the juce project, is necessary to fetch the code for it together with specifying and finding the Python install. Once both are setup they need to be passed to the list of libraries to link. 

```cmake
# Fetch the dependency using FetchContent
Include (FetchContent)
FetchContent_Declare (straw
  GIT_REPOSITORY https://github.com/kunitoki/straw.git
  GIT_TAG origin/main
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/straw)
FetchContent_MakeAvailable (straw)

# Setup the Python_ROOT_DIR where python has been installed and python options
set(Python_ROOT_DIR "/Library/Frameworks/Python.framework/Versions/Current")
set(Python_USE_STATIC_LIBS TRUE) # Optional for static builds
find_package(Python REQUIRED Development.Embed)

# Add libraries to the linking libraries of your target
target_link_libraries(${TARGET_NAME} PRIVATE
    ...
    straw::juce_straw_recommended_warning_flags
    straw::juce_straw
    Python::Python)
```

## Declare and instantiate the server

In your application, add a new dependency to `straw::AutomationServer` which will start listening on port 8001.

```cpp
class Application : public juce::JUCEApplication
{
public:
    Application() = default;

    void initialise (const String&) override
    {
        // Initialise your windows and components here...

        // Create the animation server instance
        automationServer = std::make_unique<straw::AutomationServer>();

        // Register default endpoints (optional)
        automationServer->registerDefaultEndpoints();

        // Start the HTTP server
        auto result = automationServer->start();
        if (result.failed())
            DBG(result.getErrorMessage());
    }

    void shutdown() override
    {
        // Server is stopped together with background connections
        automationServer.reset();
    }

private:
    std::unique_ptr<straw::AutomationServer> automationServer;
};
```

At this point when the app has started, it is able to respond to requests from the same machine:

```sh
curl -X GET http://localhost:8001/straw/component/exists -H 'Content-Type: application/json' -d '{"id":"MyComponentID"}'
# Will return a json with the result of the query { "result": true }
```

## Registering custom endpoints

It is possible to register custom endpoints:

```cpp
// Initialise your windows and components here...
mainComponent = std::make_unique<MainComponent>();

// Create the animation server instance
automationServer = std::make_unique<straw::AutomationServer>();

// Register a custom endpoint
auto weakMainComponent = juce::Component::SafePointer<MainComponent>(mainComponent.get());
automationServer.registerEndpoint ("/change_background_colour", [weakMainComponent](straw::Request request)
{
    // Parse the data from the request
    auto colour = juce::Colour::fromString (request.data.getProperty ("colour", "FF00FF00").toString());

    juce::MessageManager::callAsync ([weakMainComponent, colour, connection = std::move (request.connection)]
    {
        if (auto mainComponent = weakMainComponent.getComponent())
        {
            mainComponent->setColour (juce::DocumentWindow::backgroundColourId, colour);
            mainComponent->repaint();

            // Return a 200 OK HTTP response with a JSON payload of { "result": true }
            straw::sendHttpResultResponse (true, 200, *connection);
        }
        else
        {
            // Return a 404 Not Found HTTP response with a JSON payload of { "error": "..." }
            straw::sendHttpErrorMessage("Unable to find the component!", 404, *connection);
        }
    });
});

// Start the HTTP server
auto result = automationServer->start();
```

This way your application can execute a remote JSON RPC callback:

```sh
# Execute custom defined callback
curl -X GET http://localhost:8001/change_background_colour -H 'Content-Type: application/json' -d '{"colour":"FFFF0000"}
```

## Setting up and executing python test suites

To execute a test suite defined as a remote python script, all is necessary to do is to first register the default `juce` and `straw` python bindings.

```cpp

// Create the animation server instance
automationServer = std::make_unique<straw::AutomationServer>();

// Register default endpoints (needed for the remote test suite script execution)
automationServer->registerDefaultEndpoints();

// Register default bindings (juce classes and straw helper methods)
automationServer->registerDefaultComponents();

// Start the HTTP server
auto result = automationServer->start();
```

Then it's possible to send a `text/x-python` script to the automation server and it will be executed, content of `MyTestSuite.py`:

```python
import straw

straw.log ("Testing findComponent ====================")

comp = straw.findComponentById ("non-existing")
straw.assertTrue (comp is None)

comp = straw.findComponentById ("button")
straw.assertEqual (comp.typeName(), "juce::TextButton")
straw.assertEqual (comp.getComponentID(), "button")
straw.assertTrue (comp is not None)
straw.assertTrue (comp.isVisible())
straw.assertTrue (comp.isShowing())

properties = comp.getProperties()
straw.assertEqual (len (properties), 1)
straw.assertTrue ("example" in properties)
straw.assertEqual (properties["example"], 1337)

comp = straw.findComponentById ("slider")
straw.assertEqual (comp.typeName(), "juce::Slider")
straw.assertGreaterThan (comp.getValue(), 0.0)

comp = straw.findComponentById ("animation")
children = comp.getChildren()
straw.assertEqual (len (children), 2)
straw.assertTrue ([x for x in children if x.getComponentID() == "button"])
straw.assertTrue ([x for x in children if x.getComponentID() == "slider"])
```

And send it to the automation server for evaluation:

```sh
curl --data-binary '@MyTestSuite.py' http://localhost:8001 -H 'Content-Type: text/x-python'
```

If all is good, a result JSON object is returned `{ "result": true }`

## Expose custom components and custom methods to the python scripts

TODO


## Example of REST API

```sh
# Test if a component exists
curl -X GET http://localhost:8001/straw/component/exists -H 'Content-Type: application/json' -d '{"id":"animation"}'

# Return the informations from a component (recursive as well)
curl -X GET http://localhost:8001/straw/component/info -H 'Content-Type: application/json' -d '{"id":"animation", "recursive": true}'

# Click a component
curl -X GET http://localhost:8001/straw/component/click -H 'Content-Type: application/json' -d '{"id":"button"}'

# Render a component (with or without children and return a png)
curl -X GET http://localhost:8001/straw/component/render -H 'Content-Type: application/json' -d '{"id":"animation", "withChildren":true}' > test.png

# Execute custom defined callback
curl -X GET http://localhost:8001/change_background_colour -H 'Content-Type: application/json' -d '{"colour":"FFFF0000"}'
```

## Example of Python API

```sh
curl --data-binary '@./Demo/Scripts/clickComponent.py' http://localhost:8001 -H 'Content-Type: text/x-python'
curl --data-binary '@./Demo/Scripts/findComponent.py' http://localhost:8001 -H 'Content-Type: text/x-python'
curl --data-binary '@./Demo/Scripts/log.py' http://localhost:8001 -H 'Content-Type: text/x-python'
curl --data-binary '@./Demo/Scripts/raise.py' http://localhost:8001 -H 'Content-Type: text/x-python'
curl --data-binary '@./Demo/Scripts/test.py' http://localhost:8001 -H 'Content-Type: text/x-python'
```
