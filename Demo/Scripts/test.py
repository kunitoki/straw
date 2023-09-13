import straw
import custom

straw.log (1, 2, "3", ["a", "b"])

#straw.throw_exception ("xyz!")

straw.assertTrue (1)
straw.assertEqual (1, 1)

comp = straw.findComponent ("button")
print (comp)
print (comp.getComponentID())

comp = straw.findComponent ("slider")
print (comp)
print (comp.getComponentID())
