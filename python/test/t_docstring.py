#! /usr/bin/env python

import sys
import os
import shutil
from tempfile import mkdtemp
import doctest

use_matplotlib = False
try:
    import matplotlib
    matplotlib.use('Agg')
    use_matplotlib = True
except:
    pass

py_files = sys.argv[1:]

total_failure_count = 0
total_test_count = 0

# Temporary working directory for IO tests
work_dir = mkdtemp()
os.chdir(work_dir)

for py_file in py_files:
    if (not use_matplotlib) and (os.path.basename(py_file) == 'viewer.py'):
        failure_count, test_count = 0, 0
    else:
        py_basename = os.path.splitext(os.path.basename(py_file))[0]
        module = __import__('openturns.' + py_basename, fromlist=[py_basename])
        failure_count, test_count = doctest.testmod(
            module, verbose=False, optionflags=doctest.ELLIPSIS)

    total_failure_count += failure_count
    total_test_count += test_count

    print(('%s %5d tests failed'
           % ((py_basename + ' ').ljust(60, '.'), failure_count)))

print(('-' * 79))
print(('%s %5d tests failed'
       % ('TOTAL '.ljust(60, '.'), total_failure_count)))

# Delete temporary working directory for IO tests
os.chdir('..')
shutil.rmtree(work_dir)

sys.exit(total_failure_count)
