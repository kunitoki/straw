# ==============================================================================
#
#   This file is part of the straw project.
#   Copyright (c) 2024 - kunitoki@gmail.com
#
#   straw is an open source library subject to open-source licensing.
#
#   The code included in this file is provided under the terms of the ISC license
#   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
#   To use, copy, modify, and/or distribute this software for any purpose with or
#   without fee is hereby granted provided that the above copyright notice and
#   this permission notice appear in all copies.
#
#   STRAW IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
#   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
#   DISCLAIMED.
#
# ==============================================================================

import straw

def throwingMethod():
    straw.throwException ("Invalid result of the processing")

def example():
    global throwingMethod # This needs to be specified as a global because of how pybind11 behaves
    throwingMethod()

# Example of a run that is failing
straw.log ("-- pre")
example()
straw.log ("-- post")
