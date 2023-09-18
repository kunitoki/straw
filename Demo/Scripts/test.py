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
