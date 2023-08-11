
from . import auto_graph_cpp

def initialize():
	_ = auto_graph_cpp.ScopeTimer('initialize')
	auto_graph_cpp.initialize()

def task(code: str):
	_ = auto_graph_cpp.ScopeTimer('task')
	auto_graph_cpp.task(code)

def cleanup():
	_ = auto_graph_cpp.ScopeTimer('cleanup')
	auto_graph_cpp.cleanup()

def print(msg: str):
	auto_graph_cpp.print(msg)

def testGraph():
	auto_graph_cpp.testGraph()