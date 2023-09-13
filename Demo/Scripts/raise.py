import straw

# Example of a run that is failing
straw.log ("-- pre")
straw.throwException ("Invalid result of the processing")
straw.log ("-- post")
