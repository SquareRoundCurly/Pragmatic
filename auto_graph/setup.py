import sys
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from pathlib import Path
import shutil
from typing import List

source_dir          = Path(__file__).parent.absolute()
project_dir         = source_dir.parent
build_dir           = project_dir.joinpath('build')
extension_build_dir = build_dir.joinpath('lib.win-amd64-cpython-312')
external_dir        = project_dir.joinpath('external')
python_dir          = external_dir.joinpath('cpython')
python_include_dir  = python_dir.joinpath('Include')
python_lib_dir      = python_dir.joinpath('PCbuild/amd64')

module_name = 'auto_graph_cpp'
pyd_name    = f'{module_name}.cp312-win_amd64.pyd'
pyd_d_name  = f'{module_name}_d.cp312-win_amd64.pyd'

is_debug = '--debug' in sys.argv

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
		if(source_dir.joinpath(pyd_d_name).exists()):
			source_dir.joinpath(pyd_d_name).unlink()
		
		# Call the original build_ext command
		build_ext.run(self)

		# Post-build step
		print('Postbuild step')
		if is_debug:
			shutil.copy2(extension_build_dir.joinpath(pyd_d_name), source_dir.joinpath(pyd_d_name))
		else:
			shutil.copy2(extension_build_dir.joinpath(pyd_name), source_dir.joinpath(pyd_name))

module = Extension(
	module_name,
	sources = glob_files(source_dir, 'cpp'),
	include_dirs = list(map(str, [
        python_include_dir
    ])),
    library_dirs = list(map(str, [
        python_lib_dir
	])),
    libraries = [
        f'python312{"_d" if is_debug else ""}'
	],
	extra_compile_args = [
        '/std:c++20',
		'/Zi'
	],
	extra_link_args = [
		'/DEBUG'
	]
)

if '--debug' in sys.argv:
	module.name += '_d'

setup(
	name        = module_name,
	version     = '1.0',
	description = 'C++ accelerated parts of auto_graph',
	ext_modules = [module],
	cmdclass    = { 'build_ext': CustomBuildExtCommand }
)