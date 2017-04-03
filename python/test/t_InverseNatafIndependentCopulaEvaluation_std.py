#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    dim = 2
    transformation = InverseNatafIndependentCopulaEvaluation(dim)
    print("transformation=", repr(transformation))
    point = Point(dim, 0.75)
    print("transformation(", point, ")=", repr(transformation(point)))
    print("transformation parameters gradient=", repr(
        transformation.parameterGradient(point)))
    print("input dimension=", transformation.getInputDimension())
    print("output dimension=", transformation.getOutputDimension())

except:
    import sys
    print("t_InverseNatafIndependentCopulaEvaluation_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
