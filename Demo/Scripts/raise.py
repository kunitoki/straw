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

