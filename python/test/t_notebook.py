#! /usr/bin/env python

from __future__ import print_function
import os
import fnmatch
import sys
import glob
import traceback
import time

import nbformat
import nbconvert

current_path = os.path.dirname(os.path.realpath(__file__))
ipynb_path = current_path + '/../doc/examples'
print('ipynb_path=', ipynb_path)

if len(sys.argv) > 1:
    ipynbs = sys.argv[1:]
else:
    ipynbs = []
    for root, dirnames, filenames in os.walk(ipynb_path):
        for filename in fnmatch.filter(filenames, '*.ipynb'):
            ipynb = os.path.join(root, filename)
            if 'ipynb_checkpoints' in ipynb:  # exclude automatic backups
                continue
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
        t0 = time.time()
        exec(source.encode())
        elapsed = time.time() - t0
        print('--', b_name, 'OK T=', round(elapsed, 2), 's')
    except:
        cl, exc, tb = sys.exc_info()
        line_number = traceback.extract_tb(tb)[-1][1]
        line = source.split("\n")[line_number - 1]
        print ('  File "'+ipynb+'"\n   ', line)
        n_fail += 1
        print('--', b_name, '***Failed')
        traceback.print_exc()

sys.exit(n_fail)
