import auto_graph
import importlib

auto_graph.print('Hello')

importlib.reload(auto_graph)

auto_graph.print('Hello')

import sys
if 'auto_graph' in sys.modules:
    del sys.modules['auto_graph']
import auto_graph

auto_graph.print('Hello')