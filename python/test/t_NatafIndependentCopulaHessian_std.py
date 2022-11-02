#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

dim = 2
transformation = ot.NatafIndependentCopulaHessian(dim)
print("transformation=", repr(transformation))
point = ot.Point(dim, 0.75)
print("transformation(", point, ")=", repr(transformation.hessian(point)))
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())
