import unittest
import sys

class DelayedTextTestResult(unittest.TextTestResult):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)
		self.delayed_output = []

	def addSuccess(self, test):
		self.delayed_output.append('.')

	def addError(self, test, err):
		self.delayed_output.append('E')

	def addFailure(self, test, err):
		self.delayed_output.append('F')

	def addSkip(self, test, reason):
		self.delayed_output.append('s')

	def printErrors(self):
		# Print all the delayed outputs at once
		self.stream.write(''.join(self.delayed_output))
		self.stream.flush()
		super().printErrors()

class DelayedTextTestRunner(unittest.TextTestRunner):
	resultclass = DelayedTextTestResult

if __name__ == "__main__":
	test_dir = "./tests"

	loader = unittest.TestLoader()

	# Run single test or all of them through discovery
	if len(sys.argv) == 2:
		suite = loader.loadTestsFromName(sys.argv[1])
	else:
		suite = loader.discover(test_dir, pattern='*_test.py')

	# Run the tests
	runner = DelayedTextTestRunner()
	runner.run(suite)