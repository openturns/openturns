#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

dim = 2
transformation = ot.InverseNatafIndependentCopulaGradient(dim)
print("transformation=", repr(transformation))
point = ot.Point(dim, 0.75)
print("transformation(", point, ")=", repr(transformation.gradient(point).clean(1e-6)))
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())
