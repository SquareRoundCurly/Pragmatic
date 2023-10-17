import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.absolute()))

import auto_graph

hash = auto_graph.hash_utils.calculate_hash('tests/4_simple_cpp_build/src/Main.cpp')
print(hash)
auto_graph.hash_utils.store_hash_in_json('tests/4_simple_cpp_build/src/Main.cpp', 'temp.json')
print(f'Is same: {auto_graph.hash_utils.verify_file_hash("tests/4_simple_cpp_build/src/Main.cpp", "temp.json")}')