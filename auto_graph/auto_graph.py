import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent.parent.joinpath('build/lib.win-amd64-cpython-312')))

import auto_graph_cpp

print(auto_graph_cpp.add(1, 2))