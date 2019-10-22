#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Discrete related functions
    # rDiscrete
    p = [0.1, 0.2, 0.3, 0.4]
    print("rDiscrete(", Point(p), ")=",
          DistFunc.rDiscrete(p))
    print("rDiscrete(", Point(p), ", 100)=",
          DistFunc.rDiscrete(p, 100))

except:
    import sys
    print("t_DistFunc_discrete.py", sys.exc_info()[0], sys.exc_info()[1])
