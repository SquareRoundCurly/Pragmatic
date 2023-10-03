import unittest
import sys
import os
import glob
from pathlib import Path

print(f'Running tests in directory: {Path(__file__).parent}')

def find_test_module(test_number, base_dir='tests'):
	# Search for directories that match the pattern
	matching_dirs = glob.glob(os.path.join(base_dir, f"{test_number}_*"))
	
	if not matching_dirs:
		raise ValueError(f"No matching directory found for {test_number}_* pattern.")
	if len(matching_dirs) > 1:
		raise ValueError(f"Multiple directories found for {test_number}_* pattern.")
	
	test_dir = matching_dirs[0]
	
	# Search for test files that match the pattern within the found directory
	test_files = glob.glob(os.path.join(test_dir, '*_test.py'))
	
	if not test_files:
		raise ValueError(f"No test file found in directory {test_dir}.")
	if len(test_files) > 1:
		raise ValueError(f"Multiple test files found in directory {test_dir}.")
	
	# Convert file path to module format
	module_name = test_files[0].replace(os.sep, '.').replace('.py', '')
	
	return module_name

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
		test_number = int(sys.argv[1])
		test_name = find_test_module(test_number)

		suite = loader.loadTestsFromName(test_name)
	else:
		suite = loader.discover(test_dir, pattern='*_test.py')

	# Run the tests
	runner = DelayedTextTestRunner()
	runner.run(suite)

	print('Done running tests')
	exit()