import sys
from pathlib import Path

try:
	import juce
except ImportError:
	folder = Path(__file__).parent.parent / "build" / "lib.macosx-10.9-universal2-cpython-311"
	sys.path.append(str(folder.resolve()))
	import juce


class MainContentComponent(juce.Component, juce.Timer):
	def __init__(self):
		juce.Component.__init__(self)
		juce.Timer.__init__(self)

		self.random = juce.Random(13)

		self.setSize(600, 400)
		self.setOpaque(True)
		self.startTimerHz(60)

	def paint(self, g):
		g.fillAll(juce.Colour.fromRGBA(0, 0, 0, 255))

		rect = juce.Rectangle[int](0, 0, 20, 20)

		for _ in range(100):
			g.setColour(juce.Colour.fromRGBA(
				self.random.nextInt(255),
				self.random.nextInt(255),
				self.random.nextInt(255),
				255))

			rect.setCentre(self.random.nextInt(self.getWidth()), self.random.nextInt(self.getHeight()))
			g.drawRect(rect, 1)

	def timerCallback(self):
		self.repaint()


class MainWindow(juce.DocumentWindow):
	component = None

	def __init__(self):
		super().__init__(
			juce.JUCEApplication.getInstance().getApplicationName(),
			juce.Colour(0, 0, 0, 0),
			juce.DocumentWindow.allButtons,
			True)

		self.component = MainContentComponent()

		self.setResizable(True, True)
		self.setContentNonOwned(self.component, True)
		self.centreWithSize(800, 600)
		self.setVisible(True)

	def __del__(self):
		self.clearContentComponent()
		if self.component:
			del self.component

	def closeButtonPressed(self):
		juce.JUCEApplication.getInstance().systemRequestedQuit()


class MyApp(juce.JUCEApplication):
	window = None

	def __init__(self):
		super().__init__()

	def getApplicationName(self):
		return "MyApp"

	def getApplicationVersion(self):
		return "1.0"

	def initialise(self, commandLineParameters):
		print("initialise", commandLineParameters)

		self.window = MainWindow()

	def shutdown(self):
		print("shutdown")

		del self.window

	def systemRequestedQuit(self):
		print("systemRequestedQuit")
		self.quit()

	def unhandledException(self, ex, file, line):
		print(">>", ex, type(ex), file, line)
		sys.exit(1)


if __name__ == "__main__":
	juce.START_JUCE_APPLICATION(MyApp)
