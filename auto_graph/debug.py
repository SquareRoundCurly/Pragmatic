import gc
from typing import List, Any

def find_objects_from_modules(modules: List[str] = ['auto_graph', 'auto_graph_cpp']) -> List[Any]:
	def is_obj_from_module(obj: Any, modules: List[str]) -> bool:
		module_name = getattr(type(obj), '__module__', None)
		return module_name in modules
	
	gc.collect()
	objs = gc.get_objects()
	auto_graph_objects = [obj for obj in objs if is_obj_from_module(obj, modules)]
	
	for obj in auto_graph_objects:
		print(obj, type(obj))

	return auto_graph_objects