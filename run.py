import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.absolute()))

import auto_graph

auto_graph.initialize()

g = auto_graph.Graph()

g.add_node('a', 'True')
g.add_node('b', 'True')
g.add_node('c', 'True')
g.add_node('d', 'True')

g.add_edge('a', 'b', '''
print("Hello")
__return = False
''')
g.add_edge('b', 'c')
g.add_edge('b', 'd')

success = g.exec()
print(f'Graph executed: {"successfully" if success else "with errors"}')

auto_graph.cleanup()

pass