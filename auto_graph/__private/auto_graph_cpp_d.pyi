from typing import Callable, Optional, Self, List

def cleanup() -> None: ...
def reinit() -> None: ...
def print(msg: str) -> None: ...

def task(task: Callable) -> None: ...
def exec() -> None: ...

def internal_runner(): ...
def run_in_subprocess(): ...

class Task():
	def exec(): ...

class Graph():
	def add_node(name: str): ...
	def add_edge(fromName: str, toName: str): ...
	def topological_generations(): ...

class ScopeTimer():
	...