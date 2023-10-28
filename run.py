import auto_graph
import importlib
import dill
from multiprocessing import Pool

def run_in_subprocess(func, *args):
	# Ensure the function and args are serializable
	serialized_func = dill.dumps(func)
	serialized_args = dill.dumps(args)
	
	with Pool(1) as pool:  # creates a pool with one process
		serialized_result = pool.apply(auto_graph.internal_runner, (serialized_func, serialized_args))
	
	return dill.loads(serialized_result)

def _internal_runner(serialized_func, serialized_args):
	func = dill.loads(serialized_func)
	args = dill.loads(serialized_args)
	result = func(*args)
	return dill.dumps(result)

def test_function(x, y):
		return x + y

if __name__ == '__main__':
	result = auto_graph.run_in_subprocess(test_function, 1, 2)
	# result = run_in_subprocess(test_function, 1, 2)
	print(result)  # Expected to print 3