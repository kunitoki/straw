import pydoc
import juce
import straw
from io import StringIO

buffer = StringIO()
pydoc.doc (juce.Justification, output=buffer)
print (buffer.getvalue())

straw.quitApplication()
