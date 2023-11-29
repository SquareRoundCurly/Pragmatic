import auto_graph
import importlib
import dill
from multiprocessing import Pool

def _internal_runner(serialized_func, serialized_args, serialized_kwargs):
	func = dill.loads(serialized_func)
	args = dill.loads(serialized_args)
	kwargs = dill.loads(serialized_kwargs)
	result = func(*args, **kwargs)
	return dill.dumps(result)

def run_in_subprocess(func, *args, **kwargs):
	# Ensure the function and args are serializable
	serialized_func = dill.dumps(func)
	serialized_args = dill.dumps(args)
	serialized_kwargs = dill.dumps(kwargs)
	
	with Pool(1) as pool: # creates a pool with one process
		serialized_result = pool.apply(_internal_runner, (serialized_func, serialized_args, serialized_kwargs))
	
	return dill.loads(serialized_result)

def test_function(x, y=2):
		return x + y

if __name__ == '__main__':
	result = auto_graph.run_in_subprocess(test_function, 1, y=4)
	print(result)


	# Example usage with positional arguments
	result = run_in_subprocess(test_function, 1, 2)
	print(result)  # Expected to print 3

	# Example usage with keyword arguments
	result = run_in_subprocess(test_function, 1, 2)
	print(result)  # Expected to print 3