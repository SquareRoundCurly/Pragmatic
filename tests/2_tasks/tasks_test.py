
import unittest
import auto_graph


class TasksTest(unittest.TestCase):
	def test_0_empty_constructor(self):
		with self.assertRaises(Exception) as context:
			task = auto_graph.Task()
		self.assertIn('Expected at least a callable argument', str(context.exception))

	def test_1_basic_function(self):
		def test_func(i: int, j: int) -> int:
			return i + j
		t = auto_graph.Task(test_func, 1000, 2000)
		result = t.exec()
		self.assertEqual(result, 3000)

	def test_2_mixed_args(self):
		def test_func(i: int, j: int) -> int:
			return i + j
		t = auto_graph.Task(test_func, 1000)
		result = t.exec(24)
		self.assertEqual(result, 1024)

	def test_3_late_args(self):
		def test_func(i: int, j: int) -> int:
			return i + j
		t = auto_graph.Task(test_func)
		result = t.exec(1, 1000)
		self.assertEqual(result, 1001)

	def test_4_undersupply_args(self):
		def test_func(a, b, c):
			return a + b + c
		t = auto_graph.Task(test_func, 1, 2)
		with self.assertRaises(Exception) as context:
			result = t.exec()
		self.assertIn('Not enough arguments provided', str(context.exception))

	def test_5_oversupply_args(self):
		def test_func(a, b):
			return a + b
		t = auto_graph.Task(test_func, 1)
		result = t.exec(2, 3, 4)
		self.assertEqual(result, 3)

	def test_6_complex_callable(self):
		def complex_func(a: int, b: str, c: list) -> str:
			return b * a + ''.join(c)
		t = auto_graph.Task(complex_func, 3, 'hello')
		result = t.exec(['world', '!'])
		self.assertEqual(result, 'hellohellohelloworld!')

	def test_7_callable_with_default_args(self):
		def func_with_default(a, b=5):
			return a + b
		t = auto_graph.Task(func_with_default, 2)
		result = t.exec()
		self.assertEqual(result, 7)
		result_with_override = t.exec(8)
		self.assertEqual(result_with_override, 10)

	def test_8_callable_with_var_args(self):
		def var_args_func(a, *args):
			return a + sum(args)
		t = auto_graph.Task(var_args_func, 1, 2)
		result = t.exec(3, 4, 5)
		self.assertEqual(result, 15)

	def test_9_callable_with_kwargs(self):
		def kwargs_func(a, b=2, **kwargs):
			return a + b + kwargs.get('c', 0)
		t = auto_graph.Task(kwargs_func, 1)
		result = t.exec(b=3, c=4)
		self.assertEqual(result, 8)