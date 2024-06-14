/*
  ==============================================================================

   This file is part of the straw project.
   Copyright (c) 2024 - kunitoki@gmail.com

   straw is an open source library subject to open-source licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   STRAW IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
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
