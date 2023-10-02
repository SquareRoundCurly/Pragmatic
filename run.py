import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.absolute()))

import auto_graph

sleepy_code = """
import threading
import time

time.sleep(0.1)
print(f"Running on thread {threading.get_ident()}")

__return = True
"""


for i in range(20):
	# auto_graph.task('__return = True')
	auto_graph.task(sleepy_code)