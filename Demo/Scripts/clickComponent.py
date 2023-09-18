import straw

straw.log ("Testing clickComponent")

slider = straw.findComponentById ("slider")
straw.assertTrue (slider.isVisible())

straw.clickComponent ("button")
straw.assertFalse (slider.isVisible())

straw.clickComponent ("button")
straw.assertTrue (slider.isVisible())
