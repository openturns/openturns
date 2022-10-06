#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

ot.PlatformInfo.SetNumericalPrecision(4)
coll1 = ot.DistributionCollection(0)
coll1.add(ot.Normal(1.0, 2.5))
coll1.add(ot.Gamma(1.5, 3.0))
pointLow = ot.Point(0)
pointLow.add(coll1[0].computeQuantile(0.25)[0])
pointLow.add(coll1[1].computeQuantile(0.25)[0])
pointHigh = ot.Point(0)
pointHigh.add(coll1[0].computeQuantile(0.75)[0])
pointHigh.add(coll1[1].computeQuantile(0.75)[0])
coll2 = ot.DistributionCollection(0)
coll2.add(ot.Gamma(2.5, 2.0))
coll2.add(ot.Normal(3.0, 1.5))
# First, check the old constructor
evaluation = ot.MarginalTransformationEvaluation(coll1)
transformation = ot.MarginalTransformationHessian(evaluation)

print("transformation=", repr(transformation))
print("transformation.hessian(", repr(pointLow), ")=",
      repr(transformation.hessian(pointLow)))
print("finite difference hessian(", repr(pointLow), ")=", repr(
    ot.CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(pointLow)))
print("transformation.hessian(", repr(pointHigh), ")=",
      repr(transformation.hessian(pointHigh)))
print("finite difference hessian(", repr(pointHigh), ")=", repr(
    ot.CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(pointHigh)))
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())

# Second, check the constructor for old inverse transformation
evaluation = ot.MarginalTransformationEvaluation(
    coll1, ot.MarginalTransformationEvaluation.TO)
transformation = ot.MarginalTransformationHessian(evaluation)
print("transformation=", repr(transformation))
uLow = ot.Point(coll1.getSize(), 0.25)
uHigh = ot.Point(coll1.getSize(), 0.75)
print("transformation.hessian(", repr(uLow), ")=",
      repr(transformation.hessian(uLow)))
print("finite difference hessian(", repr(uLow), ")=", repr(
    ot.CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(uLow)))
print("transformation.hessian(", repr(uHigh), ")=",
      repr(transformation.hessian(uHigh)))
print("finite difference hessian(", repr(uHigh), ")=", repr(
    ot.CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(uHigh)))
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())

# Third, check the constructor for the new transformation

evaluation = ot.MarginalTransformationEvaluation(coll1, coll2)
transformation = ot.MarginalTransformationHessian(evaluation)
print("transformation=", repr(transformation))
print("transformation.hessian(", repr(pointLow), ")=",
      repr(transformation.hessian(pointLow)))
print("finite difference hessian(", repr(pointLow), ")=", repr(
    ot.CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(pointLow)))
print("transformation.hessian(", repr(pointHigh), ")=",
      repr(transformation.hessian(pointHigh)))
print("finite difference hessian(", repr(pointHigh), ")=", repr(
    ot.CenteredFiniteDifferenceHessian(1.0e-4, evaluation).hessian(pointHigh)))
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())
