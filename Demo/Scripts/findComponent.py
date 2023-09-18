import straw

# Example of a run that is finding components
straw.log ("Testing findComponent")

comp = straw.findComponentById ("non-existing")
straw.assertTrue (comp is None)

comp = straw.findComponentById ("button")
straw.assertEqual (comp.typeName(), "juce::TextButton")
straw.assertEqual (comp.getComponentID(), "button")
straw.assertTrue (comp is not None)
straw.assertTrue (comp.isVisible())
straw.assertTrue (comp.isShowing())

properties = comp.getProperties()
straw.assertEqual (len (properties), 1)
straw.assertTrue ("example" in properties)
straw.assertEqual (properties["example"], 1337)
#straw.log (properties)

comp = straw.findComponentById ("slider")
straw.assertEqual (comp.typeName(), "CustomSlider")
straw.assertEqual (comp.customMethod(), "Called customMethod !")

comps = straw.findComponentsByType ("CustomSlider")
straw.assertEqual (len (comps), 1)
straw.assertEqual (comps[0].getComponentID(), comp.getComponentID())

comp = straw.findComponentById ("animation")
children = comp.getChildren()
straw.assertEqual (len (children), 2)
straw.assertTrue ([x for x in children if x.getComponentID() == "button"])
straw.assertTrue ([x for x in children if x.getComponentID() == "slider"])
