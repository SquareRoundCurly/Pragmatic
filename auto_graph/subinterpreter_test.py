from __future__ import print_function
import sys
import threading

print(f"Running on thread {threading.get_ident()}")

print("{}: sys.xxx={}".format(TNAME, getattr(sys, 'xxx', 'attribute not set')))