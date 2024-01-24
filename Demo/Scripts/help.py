import pydoc
from io import StringIO

import popsicle as juce
import straw

buffer = StringIO()
pydoc.doc (juce.Justification, output=buffer)
print (buffer.getvalue())

straw.quitApplication()
