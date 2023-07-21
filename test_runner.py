import unittest
import sys

if __name__ == "__main__":
	test_dir = "./tests"

	loader = unittest.TestLoader()

	# Run single test or all of them through discovery
	if len(sys.argv) == 2:
		suite = loader.loadTestsFromName(sys.argv[1])
	else:
		suite = loader.discover(test_dir, pattern='*.py')

	# Run the tests
	runner = unittest.TextTestRunner()
	runner.run(suite)