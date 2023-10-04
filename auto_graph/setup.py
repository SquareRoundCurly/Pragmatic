import sys
import os
from pathlib import Path
import shutil
from typing import List
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

is_windows = sys.platform == "win32"
is_linux = sys.platform == "linux"
is_debug_build = '--debug' in sys.argv
is_debug_python = hasattr(sys, 'gettotalrefcount')

source_dir          = Path(__file__).parent.absolute()
cpp_source_dir      = source_dir.joinpath('auto_graph_cpp')
project_dir         = source_dir.parent
build_dir           = project_dir.joinpath('build')
extension_build_dir = build_dir.joinpath(f'{"lib.win-amd64" if is_windows else "lib.linux-x86_64"}-cpython-312{"-pydebug" if is_debug_python else ""}')
external_dir        = project_dir.joinpath('external')
python_dir          = external_dir.joinpath('cpython')
python_include_dir  = python_dir.joinpath('Include')
python_lib_dir      = python_dir.joinpath('PCbuild/amd64') if is_windows else python_dir

# If building with debug version of Python, '_d' is already appended.
# If building with release version of Python,
# but in debug configuration for this extension, still use '_d'. 
module_name = f'auto_graph_cpp{"_d" if is_windows and (is_debug_build and not is_debug_python) else ""}'
pyd_name    = f'{module_name}{"_d" if is_windows and is_debug_python else ""}.{"cp312-win_amd64.pyd" if is_windows else "cpython-312d-x86_64-linux-gnu.so"}' # TODO: handle linux release python, release build

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
		if(source_dir.joinpath(pyd_name).exists()):
			source_dir.joinpath(pyd_name).unlink()
		
		# Call the original build_ext command
		build_ext.run(self)

		# Post-build step
		print('Postbuild step')
		src = extension_build_dir.joinpath(pyd_name)
		dst = source_dir.joinpath(pyd_name)
		print(f'copy: \n src: {src}\n dst: {dst}')
		shutil.copy2(src, dst)

def compile_args():
	if is_windows:
		return [
			'/std:c++20',
			'/Zi'
		]
	elif is_linux:
		return [
			'-std=c++20',
			'-g',
			'-O0'
		]

module = Extension(
	module_name,
	sources = glob_files(cpp_source_dir, 'cpp'),
	depends = glob_files(cpp_source_dir, 'hpp'),
	include_dirs = list(map(str, [
		python_include_dir,
		external_dir.joinpath('readerwriterqueue'),
		external_dir.joinpath('concurrentqueue')
	])),
	library_dirs = list(map(str, [
		python_lib_dir
	])),
	libraries = [f'python312{"_d" if is_debug_build else ""}'] if is_windows else [] # [f'python3.12{"d" if is_debug_build else ""}']
	,
	extra_compile_args = compile_args(),
	extra_link_args = ['/DEBUG'] if is_windows else []
)

setup(
	name        = module_name,
	version     = '1.0',
	description = 'C++ accelerated parts of auto_graph',
	ext_modules = [module],
	cmdclass    = { 'build_ext': CustomBuildExtCommand }
)