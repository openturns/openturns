#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    PlatformInfo.SetNumericalPrecision(4)
    coll1 = DistributionCollection(0)
    coll1.add(Normal(1.0, 2.5))
    coll1.add(Gamma(1.5, 3.0))
    pointLow = Point(0)
    pointLow.add(coll1[0].computeQuantile(0.25)[0])
    pointLow.add(coll1[1].computeQuantile(0.25)[0])
    pointHigh = Point(0)
    pointHigh.add(coll1[0].computeQuantile(0.75)[0])
    pointHigh.add(coll1[1].computeQuantile(0.75)[0])
    coll2 = DistributionCollection(0)
    coll2.add(Gamma(2.5, 2.0))
    coll2.add(Normal(3.0, 1.5))
    # First, check the old constructor
    evaluation = MarginalTransformationEvaluation(coll1)
    transformation = MarginalTransformationHessian(evaluation)

    print("transformation=", repr(transformation))
    print("transformation.hessian(", repr(pointLow), ")=",
          repr(transformation.hessian(pointLow)))
    print("finite difference hessian(", repr(pointLow), ")=", repr(
        CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(pointLow)))
    print("transformation.hessian(", repr(pointHigh), ")=",
          repr(transformation.hessian(pointHigh)))
    print("finite difference hessian(", repr(pointHigh), ")=", repr(
        CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(pointHigh)))
    print("input dimension=", transformation.getInputDimension())
    print("output dimension=", transformation.getOutputDimension())

    # Second, check the constructor for old inverse transformation
    evaluation = MarginalTransformationEvaluation(
        coll1, MarginalTransformationEvaluation.TO)
    transformation = MarginalTransformationHessian(evaluation)
    print("transformation=", repr(transformation))
    uLow = Point(coll1.getSize(), 0.25)
    uHigh = Point(coll1.getSize(), 0.75)
    print("transformation.hessian(", repr(uLow), ")=",
          repr(transformation.hessian(uLow)))
    print("finite difference hessian(", repr(uLow), ")=", repr(
        CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(uLow)))
    print("transformation.hessian(", repr(uHigh), ")=",
          repr(transformation.hessian(uHigh)))
    print("finite difference hessian(", repr(uHigh), ")=", repr(
        CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(uHigh)))
    print("input dimension=", transformation.getInputDimension())
    print("output dimension=", transformation.getOutputDimension())

    # Third, check the constructor for the new transformation

    evaluation = MarginalTransformationEvaluation(coll1, coll2)
    transformation = MarginalTransformationHessian(evaluation)
    print("transformation=", repr(transformation))
    print("transformation.hessian(", repr(pointLow), ")=",
          repr(transformation.hessian(pointLow)))
    print("finite difference hessian(", repr(pointLow), ")=", repr(
        CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(pointLow)))
    print("transformation.hessian(", repr(pointHigh), ")=",
          repr(transformation.hessian(pointHigh)))
    print("finite difference hessian(", repr(pointHigh), ")=", repr(
        CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(pointHigh)))
    print("input dimension=", transformation.getInputDimension())
    print("output dimension=", transformation.getOutputDimension())

except:
    import sys
    print("t_MarginalTransformationHessian_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
