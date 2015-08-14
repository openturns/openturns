#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:
    b = Basis()
    seq = BasisSequence(b)
    print("sequence =", seq)

except:
    import sys
    print("t_BasisSequence_std.py", sys.exc_info()[0], sys.exc_info()[1])
