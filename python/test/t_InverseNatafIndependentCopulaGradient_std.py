#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    dim = 2
    transformation = InverseNatafIndependentCopulaGradient(dim)
    print("transformation=", repr(transformation))
    point = Point(dim, 0.75)
    print("transformation(", point, ")=", repr(
        transformation.gradient(point).clean(1e-6)))
    print("input dimension=", transformation.getInputDimension())
    print("output dimension=", transformation.getOutputDimension())

except:
    import sys
    print("t_InverseNatafIndependentCopulaGradient_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
