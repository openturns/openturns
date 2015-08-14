#! /usr/bin/env python

from __future__ import print_function
import os
import sys
from openturns import *

Log.Show(Log.ALL)

TESTPREAMBLE()

try:
    # Template input files
    f = open('poutre_external_infile1', 'w')
    f.write('F=1000\nE=120E9\n')
    f.close()

    f = open('poutre_external_infile2', 'w')
    f.write('L=1.5\nI=2E-6\n')
    f.close()

    # Instance creation
    system = 'LINUX'
    if 'win' in sys.platform:
        system = 'WIN32'
    deviation = NumericalMathFunction('poutre_external_' + system)

    inPoint = NumericalPoint([210.e9, 1000, 1.5, 2.e-6])

    outPoint = deviation(inPoint)

    print("deviation =", repr(outPoint))

    # cleanup
    os.remove('poutre_external_infile1')
    os.remove('poutre_external_infile2')

except:
    import sys
    import traceback
    traceback.print_exc(file=sys.stdout)
