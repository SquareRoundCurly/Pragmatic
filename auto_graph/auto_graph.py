
from . import auto_graph_cpp

def task(code: str):
	auto_graph_cpp.task(code)

def cleanup():
	auto_graph_cpp.cleanup()