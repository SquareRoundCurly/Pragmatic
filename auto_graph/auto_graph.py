from . import auto_graph_cpp

import atexit
atexit.register(auto_graph_cpp.cleanup)

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

def run_command(cmd: str):
	_ = auto_graph_cpp.ScopeTimer('run_command')
	output = auto_graph_cpp.run_command(cmd)
	return output