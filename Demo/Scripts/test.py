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
import custom

straw.log (1, 2, "3", ["a", "b"])

# straw.throwException ("xyz!")

straw.assertTrue (1)
straw.assertFalse (0)
straw.assertEqual (1, 1)
straw.assertLessThan (1, 2)
straw.assertLessThanEqual (1, 1)
straw.assertGreaterThan (2, 1)
straw.assertGreaterThanEqual (1, 1)

comp = straw.findComponentById ("button")
print (comp)
print (comp.getComponentID())

comp = straw.findComponentById ("slider")
print (comp)
print (comp.getComponentID())
