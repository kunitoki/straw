/**
 * straw 4 the juce - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "straw_ComponentEndpoints.h"

#include "../helpers/straw_ComponentHelpers.h"

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>

namespace straw::Endpoints {

//=================================================================================================

void componentExists (Request request)
{
    auto componentID = request.data.getProperty ("id", "").toString().trim();
    if (componentID.isEmpty())
    {
        sendHttpErrorMessage ("invalid component id specified", 500, *request.connection);
        return;
    }

    juce::MessageManager::callAsync ([componentID, connection = std::move (request.connection)]
    {
        juce::Component* foundComponent = Helpers::findComponentById (componentID);

        juce::var response = makeResultVar (foundComponent != nullptr);
        sendHttpResponse (response, 200, *connection);
    });
}

//=================================================================================================

void componentVisible (Request request)
{
    auto componentID = request.data.getProperty ("id", "").toString().trim();
    if (componentID.isEmpty())
    {
        sendHttpErrorMessage ("invalid component id specified", 500, *request.connection);
        return;
    }

    juce::MessageManager::callAsync ([componentID, connection = std::move (request.connection)]
    {
        juce::Component* foundComponent = Helpers::findComponentById (componentID);

        juce::var response = makeResultVar (foundComponent != nullptr && foundComponent->isVisible());
        sendHttpResponse (response, 200, *connection);
    });
}

//=================================================================================================

void componentInfo (Request request)
{
    auto componentID = request.data.getProperty ("id", "").toString().trim();
    if (componentID.isEmpty())
    {
        sendHttpErrorMessage("invalid component id specified", 500, *request.connection);
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
        sendHttpErrorMessage ("invalid component id specified", 500, *request.connection);
        return;
    }

    auto clickTime = static_cast<int> (request.data.getProperty ("time", 100));
    
    juce::MessageManager::callAsync([componentID, clickTime, request = std::move (request)]
    {
        Helpers::clickComponent (componentID, juce::ModifierKeys(), [request = std::move (request)]
        {
            juce::var response = makeResultVar (true);
            sendHttpResponse (response, 200, *request.connection);
        }, juce::RelativeTime::milliseconds(clickTime));
    });
}

//=================================================================================================

void componentRender (Request request)
{
    auto componentID = request.data.getProperty ("id", "").toString().trim();
    if (componentID.isEmpty())
    {
        sendHttpErrorMessage ("invalid component id specified", 500, *request.connection);
        return;
    }

    auto withChildren = static_cast<bool> (request.data.getProperty ("withChildren", false));

    juce::MessageManager::callAsync([componentID, withChildren, request = std::move (request)]
    {
        if (juce::Component* foundComponent = Helpers::findComponentById (componentID))
        {
            auto image = Helpers::renderComponentToImage (foundComponent, withChildren);
            sendHttpResponse (image, 200, *request.connection);
        }
        else
        {
            sendHttpErrorMessage("component id not found", 500, *request.connection);
        }
    });
}

} // namespace straw::Endpoints
