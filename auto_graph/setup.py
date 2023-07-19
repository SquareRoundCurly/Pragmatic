import sys
from setuptools import setup, Extension
from pathlib import Path
from typing import List

source_dir         = Path(__file__).parent.absolute()
project_dir        = source_dir.parent
external_dir       = project_dir.joinpath('external')
python_dir         = external_dir.joinpath('cpython')
python_include_dir = python_dir.joinpath('Include')
python_lib_dir     = python_dir.joinpath('PCbuild/amd64')

module_name = 'auto_graph_cpp'

def glob_files(directory: Path, extension: str) -> List[Path]:
    """Globs all '.extension' files recursively in a given directory"""
    return [str(path) for path in Path(directory).rglob(f'*.{extension}')]

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
        'python312_d'
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
	ext_modules = [module]
)