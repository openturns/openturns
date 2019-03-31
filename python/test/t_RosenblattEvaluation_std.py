#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

nu = 4.5
dim = 5
distribution = ot.Student(nu, dim)
transformation = ot.RosenblattEvaluation(distribution)
print("transformation=", repr(transformation))
point = ot.Point(dim, 0.75)
print("transformation(", point, ")=", repr(transformation(point)))
print("transformation parameters gradient=", repr(
    transformation.parameterGradient(point)))
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())
