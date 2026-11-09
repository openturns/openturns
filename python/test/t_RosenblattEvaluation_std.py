#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

nu = 4.5
dim = 5
distribution = ot.Student(nu, dim)
transformation = ot.RosenblattEvaluation(distribution)
print("transformation=", repr(transformation))
point = ot.Point(dim, 0.75)
print("transformation(", point, ")=", repr(transformation(point)))
print(
    "transformation parameters gradient=", repr(transformation.parameterGradient(point))
)
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())

# parameters are exposed and honored
distribution = ot.Normal(1.0, 2.0)
transformation = ot.RosenblattEvaluation(distribution)
ott.assert_almost_equal(transformation.getParameter(), [1.0, 2.0])
ott.assert_almost_equal(transformation([0.0]), [-0.5])
transformation.setParameter([5.0, 7.0])
ott.assert_almost_equal(transformation.getParameter(), [5.0, 7.0])
ott.assert_almost_equal(transformation([0.0]), [-5.0 / 7.0])
ott.assert_almost_equal(
    transformation.parameterGradient([0.0]), ot.Matrix([[-1.0 / 7.0], [5.0 / 49.0]])
)
