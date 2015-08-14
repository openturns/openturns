#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:
    distribution = Triangular(-1.0, 0.3, 1.0)
    algo = OrthonormalizationAlgorithm(distribution)
    print("algo=", algo)
    print("measure=", repr(algo.getMeasure()))
    algo.setMeasure(Triangular(-1.0, -0.2, 1.0))
    print("new measure=", repr(algo.getMeasure()))

except:
    import sys
    print("t_OrthonormalizationAlgorithm_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
