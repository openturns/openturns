#! /usr/bin/env python

from __future__ import print_function
import os
import fnmatch
import sys
import glob
import traceback

import nbformat
import nbconvert

current_path = os.path.dirname(os.path.realpath(__file__))
ipynb_path = current_path + '/../doc/examples'
print('ipynb_path=', ipynb_path)
ipynbs = []
for root, dirnames, filenames in os.walk(ipynb_path):
    for filename in fnmatch.filter(filenames, '*.ipynb'):
        ipynb = os.path.join(root, filename)
        if not 'ipynb_checkpoints' in ipynb:# exclude automatic backups
            ipynbs.append(ipynb)

n_fail = 0
for ipynb in ipynbs:
    with open(ipynb) as fh:
        nb = nbformat.reads(fh.read(), 4)

    exporter = nbconvert.PythonExporter()

    # source is a tuple of python source code
    # meta contains metadata
    source, meta = exporter.from_notebook_node(nb)

    b_name = os.path.basename(ipynb)
    print('--', b_name)
    try:
        exec(source.encode())
        print('--', b_name, 'OK')
    except:
        n_fail += 1
        print('--', b_name, '***Failed')
        traceback.print_exc()

sys.exit(n_fail)
