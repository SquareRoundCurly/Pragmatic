import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.absolute()))

import auto_graph

file = 'tests/4_simple_cpp_build/src/Main.cpp'
json_file = 'temp.json'
is_valid = auto_graph.hash_utils.verify_file_hash(file, json_file)
print(f'Is same: {is_valid}')
if not is_valid:
	auto_graph.hash_utils.store_hash_in_json(file, json_file)
