#! /usr/bin/env python

from openturns import *
from math import *

TESTPREAMBLE()

try:
    f = LeastSquaresMetaModelSelectionFactory(LARS(), CorrectedLeaveOneOut())
    print("factory =", f)

except:
    import sys
    print("t_LeastSquaresMetaModelSelectionFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
