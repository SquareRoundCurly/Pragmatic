import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.absolute()))

import auto_graph

auto_graph.initialize()

node = auto_graph.Node('a', 'print("hello")')
node.__exec()

auto_graph.cleanup()

pass