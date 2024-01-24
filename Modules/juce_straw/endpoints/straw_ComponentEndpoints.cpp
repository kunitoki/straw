/**
 * straw 4 the juce - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#include "straw_ComponentEndpoints.h"

#include "../helpers/straw_ComponentHelpers.h"

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>

namespace straw::Endpoints {

//=================================================================================================

void sleep (Request request)
{
    auto sleepTime = static_cast<int> (request.data.getProperty ("time", 100));
    auto messageThread = static_cast<bool> (request.data.getProperty ("messageThread", false));

    auto sleeperFunction = [sleepTime] { juce::Thread::sleep(sleepTime); };

    if (messageThread)
        juce::MessageManager::callAsync (sleeperFunction);
    else
        sleeperFunction();
}

//=================================================================================================

void componentExists (Request request)
{
    auto componentID = request.data.getProperty ("id", "").toString().trim();
    if (componentID.isEmpty())
    {
        sendHttpErrorResponse ("invalid component id specified", 500, *request.connection);
        return;
    }

    juce::MessageManager::callAsync ([componentID, connection = std::move (request.connection)]
    {
        juce::Component* foundComponent = Helpers::findComponentById (componentID);

        sendHttpResultResponse (foundComponent != nullptr, 200, *connection);
    });
}

//=================================================================================================

void componentVisible (Request request)
{
    auto componentID = request.data.getProperty ("id", "").toString().trim();
    if (componentID.isEmpty())
    {
        sendHttpErrorResponse ("invalid component id specified", 500, *request.connection);
        return;
    }

    juce::MessageManager::callAsync ([componentID, connection = std::move (request.connection)]
    {
        juce::Component* foundComponent = Helpers::findComponentById (componentID);

        sendHttpResultResponse (foundComponent != nullptr && foundComponent->isVisible(), 200, *connection);
    });
}

//=================================================================================================

void componentInfo (Request request)
{
    auto componentID = request.data.getProperty ("id", "").toString().trim();
    if (componentID.isEmpty())
    {
        sendHttpErrorResponse ("invalid component id specified", 500, *request.connection);
        return;
    }

    auto recursive = static_cast<bool> (request.data.getProperty ("recursive", false));

    juce::MessageManager::callAsync ([componentID, recursive, connection = std::move (request.connection)]
    {
        juce::Component* foundComponent = Helpers::findComponentById (componentID);

        juce::var response = Helpers::makeComponentInfo (foundComponent, recursive);
        sendHttpResponse (response, 200, *connection);
    });
}

//=================================================================================================

void componentClick (Request request)
{
    auto componentID = request.data.getProperty ("id", "").toString().trim();
    if (componentID.isEmpty())
    {
        sendHttpErrorResponse ("invalid component id specified", 500, *request.connection);
        return;
    }

    auto clickTime = static_cast<int> (request.data.getProperty ("time", 100));

    juce::MessageManager::callAsync([componentID, clickTime, request = std::move (request)]
    {
        if (juce::Component* component = Helpers::findComponentById (componentID))
        {
            Helpers::clickComponent (component, juce::ModifierKeys(), [request = std::move (request)]
            {
                sendHttpResultResponse (true, 200, *request.connection);
            }, juce::RelativeTime::milliseconds(clickTime));
        }
        else
        {
            sendHttpErrorResponse("component id not found", 500, *request.connection);
        }
    });
}

//=================================================================================================

void componentRender (Request request)
{
    auto componentID = request.data.getProperty ("id", "").toString().trim();
    if (componentID.isEmpty())
    {
        sendHttpErrorResponse ("invalid component id specified", 500, *request.connection);
        return;
    }

    auto withChildren = static_cast<bool> (request.data.getProperty ("withChildren", false));

    juce::MessageManager::callAsync([componentID, withChildren, request = std::move (request)]
    {
        if (juce::Component* component = Helpers::findComponentById (componentID))
        {
            auto image = Helpers::renderComponentToImage (component, withChildren);
            sendHttpResponse (image, 200, *request.connection);
        }
        else
        {
            sendHttpErrorResponse("component id not found", 500, *request.connection);
        }
    });
}

} // namespace straw::Endpoints
