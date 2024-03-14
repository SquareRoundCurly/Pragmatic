from distutils.core import setup, Extension

auto_graph_module = Extension('auto_graph', sources=['auto_graph.cpp'])

setup(
	name='auto_graph',
	version='1.0',
	description='Python Package with a C++ extension module',
	ext_modules=[auto_graph_module]
	)
