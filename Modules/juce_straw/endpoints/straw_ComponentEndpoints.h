/**
 * straw 4 the juce - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#include "../server/straw_Request.h"

namespace straw::Endpoints {

//=================================================================================================

void sleep (Request request);

//=================================================================================================

void componentExists (Request request);
void componentVisible (Request request);
void componentInfo (Request request);
void componentClick (Request request);
void componentRender (Request request);

} // namespace straw::Endpoints
