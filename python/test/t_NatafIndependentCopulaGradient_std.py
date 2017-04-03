#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    dim = 2
    transformation = NatafIndependentCopulaGradient(dim)
    print("transformation=", repr(transformation))
    point = Point(dim, 0.75)
    print("transformation(", point, ")=", repr(transformation.gradient(point)))
    print("input dimension=", transformation.getInputDimension())
    print("output dimension=", transformation.getOutputDimension())

except:
    import sys
    print("t_NatafIndependentCopulaGradient_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
