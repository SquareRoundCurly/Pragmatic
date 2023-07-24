
from . import auto_graph_cpp

def initialize():
	auto_graph_cpp.initialize()

def task(code: str):
	auto_graph_cpp.task(code)

def cleanup():
	auto_graph_cpp.cleanup()