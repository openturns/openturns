#!/usr/bin/env python3
"""
Create a sphinx build environment for the OpenTURNS documentation
with only some of the source files in order to accelerate Sphinx builds.
Sphinx is then called to build the HTML doc using all available cores.
HTML files are written to a subfolder of the build folder called `install`.

Parameters
----------
build_name : str
    Name of the build folder

source_files : sequence of str
    Names of all source example Python scripts and RST source files to be included.

Examples
--------

Create a build directory `build-sphinxonly` that will contain only source files
required by Sphinx.

$ utils/docfast.py build-sphinxonly

Create a build directory `build-sphinxonly` that will contain only the mandatory
source files plus the `plot_smoothing_mixture.py` and `base_objects.rst` files.

$ utils/docfast.py build-sphinxonly base_objects.rst plot_smoothing_mixture.py

"""
import os
import sys
import shutil
import glob
from sphinx.cmd import build

list_preserved_files = sys.argv[2:]

script_folder = os.path.dirname(os.path.abspath(__file__)) # openturns/utils
sources_folder = os.path.dirname(script_folder) # root folder openturns
doc_folder = os.path.join(sources_folder, 'python', 'doc') # openturns/python/doc

build_folder = os.path.join(sources_folder, sys.argv[1])

shutil.copytree(doc_folder, build_folder, dirs_exist_ok=True) # Python 3.8+

# Remove all Python scripts in the examples folder...
for pypath in glob.iglob(os.path.join(build_folder, 'examples', '**', '*.py'), recursive=True):
    # ...unless their name was passed as argument.
    if os.path.basename(pypath) not in list_preserved_files:
        os.remove(pypath)

list_rstsections = ['developer_guide',
                    'theory',
                    'usecases',
                    'user_manual']

# For every doc section listed...
for section in list_rstsections:
    # ...remove all RST files...
    for rstpath in glob.iglob(os.path.join(build_folder, section, '**', '*.rst'), recursive=True):
        # ...that do not have the same name as their directory...
        if os.path.basename(rstpath)!=os.path.basename(os.path.dirname(rstpath)) + '.rst':
            # ...unless their name was passed as argument.
            if os.path.basename(rstpath) not in list_preserved_files:
                os.remove(rstpath)

# Rename conf.py.in -> conf.py
os.rename(os.path.join(build_folder, 'conf.py.in'), os.path.join(build_folder, 'conf.py'))

build.build_main(['-j',  'auto', build_folder, os.path.join(build_folder, 'install')])
