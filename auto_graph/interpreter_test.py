from __future__ import print_function
import sys
import threading

print(f"Running on thread {threading.get_ident()}")

sys.xxx = ['abc']
print('main: setting sys.xxx={}'.format(sys.xxx))