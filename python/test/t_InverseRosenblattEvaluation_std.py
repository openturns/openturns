#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

nu = 4.5
dim = 5
distribution = ot.Student(nu, dim)
transformation = InverseRosenblattEvaluation(distribution)
print("transformation=", repr(transformation))
point = Point(dim, 0.75)
print("transformation(", point, ")=", repr(transformation(point)))
print("transformation parameters gradient=", repr(
    transformation.parameterGradient(point)))
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())
