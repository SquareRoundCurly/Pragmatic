import sys
import os
from pathlib import Path
import shutil
from typing import List
from setuptools import setup, Extension, find_namespace_packages
from setuptools.command.build_ext import build_ext
import sysconfig

python_version = f"{sys.version_info.major}{sys.version_info.minor}"
is_windows = sys.platform == "win32"
is_linux = sys.platform == "linux"
is_debug_build = '--debug' in sys.argv
is_debug_python = hasattr(sys, 'gettotalrefcount')

python_lib_name = f'python{python_version}{"_d" if is_debug_build else ""}.lib' if is_windows else f'libpython{python_version}.a'

def get_python_lib_dir() -> Path:
	"""Retrieve the directory containing the Python static library."""
	python_install_dir = Path(sys.exec_prefix)

	# List of potential directories containing the Python static library
	potential_lib_dirs = []
	if is_windows:
		potential_lib_dirs = [
			python_install_dir.joinpath('libs'), 
			python_install_dir.joinpath('PCBuild/amd64')
		]
	if is_linux:
		potential_lib_dirs = [
			python_install_dir.joinpath('lib')
		]

	# Find the directory containing the Python version-specific static library
	for potential_dir in potential_lib_dirs:
		if potential_dir.joinpath(python_lib_name).exists():
			return potential_dir

	# If the library directory was not found, raise a ValueError
	raise ValueError(f"Python static library directory for {python_version} not found.")

project_dir            = Path(__file__).parent
auto_graph_dir         = project_dir.joinpath('auto_graph')
auto_graph_private_dir = auto_graph_dir.joinpath('__private')
auto_graph_cpp_dir     = project_dir.joinpath('auto_graph_cpp')
build_dir              = project_dir.joinpath('build')
extension_build_dir    = build_dir.joinpath(f'{"lib.win-amd64" if is_windows else "lib.linux-x86_64"}-cpython-{python_version}{"-pydebug" if is_debug_python else ""}').joinpath('auto_graph').joinpath('__private')
external_dir           = project_dir.joinpath('external')
python_dir             = external_dir.joinpath('cpython')
python_include_dir     = Path(sysconfig.get_path("include"))
python_lib_dir         = get_python_lib_dir()
print(python_lib_dir)

# If building with debug version of Python, '_d' is already appended.
# If building with release version of Python,
# but in debug configuration for this extension, still use '_d'. 
module_name = f'auto_graph_cpp{"_d" if is_windows and (is_debug_build and not is_debug_python) else ""}'
pyd_name    = f'{module_name}{"_d" if is_windows and is_debug_python else ""}.{f"cp{python_version}-win_amd64.pyd" if is_windows else f"cpython-{python_version}d-x86_64-linux-gnu.so"}'  # TODO: handle linux release python, release build

print(f'Building auto_graph_cpp on {"windows" if is_windows else "linux"} ...')
print(f'Using {"debug" if is_debug_python else "release"} Python')
print(f'Building {"debug" if is_debug_build else "release"} build')

if build_dir.exists():
	shutil.rmtree(build_dir)

def glob_files(directory: Path, extension: str) -> List[Path]:
	"""Globs all '.extension' files recursively in a given directory"""
	return [str(path) for path in Path(directory).rglob(f'*.{extension}')]

class CustomBuildExtCommand(build_ext):
	"""Custom build command."""

	def run(self):
		# Pre-build step
		print('Prebuild step')
		if(auto_graph_dir.joinpath(pyd_name).exists()):
			auto_graph_dir.joinpath(pyd_name).unlink()
		
		# Call the original build_ext command
		build_ext.run(self)

		# Post-build step
		print('Postbuild step')
		src = extension_build_dir.joinpath(pyd_name)
		dst = auto_graph_private_dir.joinpath(pyd_name)
		print(f'copy: \n src: {src}\n dst: {dst}')
		shutil.copy2(src, dst)

def compile_args():
	if is_windows and is_debug_build:
		return [
			'/std:c++20',
			'/Zi'
		]
	elif is_windows:
		return [
			'/std:c++20'
		]
	elif is_linux:
		return [
			'-std=c++20',
			'-g',
			'-O0'
		]

auto_graph_cpp = Extension(
	f'auto_graph.__private.auto_graph_cpp',
	sources = glob_files(auto_graph_cpp_dir, 'cpp'),
	depends = glob_files(auto_graph_cpp_dir, 'hpp'),
	include_dirs = list(map(str, [
		python_include_dir,
		external_dir.joinpath('readerwriterqueue'),
		external_dir.joinpath('concurrentqueue')
	])),
	library_dirs = list(map(str, [
		python_lib_dir
	])),
	libraries = [Path(python_lib_name).stem]
	,
	extra_compile_args = compile_args(),
	extra_link_args = ['/DEBUG'] if is_windows and (is_debug_build or is_debug_python) else []
)
print(auto_graph_cpp.extra_compile_args)

setup(
	name         = 'pragmatic',
	version      = '0.3',
	description  = 'C++ accelerated parts of auto_graph',
	packages     = ['auto_graph', 'auto_graph.__private'],
	ext_modules  = [auto_graph_cpp],
	package_data = { 'auto_graph.__private': ['*.pyd', '*.pyi'] },
	cmdclass     = { 'build_ext': CustomBuildExtCommand }
)