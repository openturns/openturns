#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

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
transformation = ot.MarginalTransformationGradient(evaluation)

print("transformation=", repr(transformation))
print("transformation.gradient(", repr(pointLow), ")=",
      repr(transformation.gradient(pointLow)))
print("finite difference gradient(", repr(pointLow), ")=", repr(
    ot.CenteredFiniteDifferenceGradient(1.0e-5, evaluation).gradient(pointLow)))
print("transformation.gradient(", repr(pointHigh), ")=",
      repr(transformation.gradient(pointHigh)))
print("finite difference gradient(", repr(pointHigh), ")=", repr(
    ot.CenteredFiniteDifferenceGradient(1.0e-5, evaluation).gradient(pointHigh)))
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())

# Second, check the constructor for old inverse transformation
evaluation = ot.MarginalTransformationEvaluation(
    coll1, ot.MarginalTransformationEvaluation.TO)
transformation = ot.MarginalTransformationGradient(evaluation)
print("transformation=", repr(transformation))
uLow = ot.Point(coll1.getSize(), 0.25)
uHigh = ot.Point(coll1.getSize(), 0.75)
print("transformation.gradient(", repr(uLow), ")=",
      repr(transformation.gradient(uLow)))
print("finite difference gradient(", repr(uLow), ")=", repr(
    ot.CenteredFiniteDifferenceGradient(1.0e-5, evaluation).gradient(uLow)))
print("transformation.gradient(", repr(uHigh), ")=",
      repr(transformation.gradient(uHigh)))
print("finite difference gradient(", repr(uHigh), ")=", repr(
    ot.CenteredFiniteDifferenceGradient(1.0e-5, evaluation).gradient(uHigh)))
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())

# Third, check the constructor for the new transformation
evaluation = ot.MarginalTransformationEvaluation(coll1, coll2)
transformation = ot.MarginalTransformationGradient(evaluation)
print("transformation=", repr(transformation))
print("transformation.gradient(", repr(pointLow), ")=",
      repr(transformation.gradient(pointLow)))
print("finite difference gradient(", repr(pointLow), ")=", repr(
    ot.CenteredFiniteDifferenceGradient(1.0e-5, evaluation).gradient(pointLow)))
print("transformation.gradient(", repr(pointHigh), ")=",
      repr(transformation.gradient(pointHigh)))
print("finite difference gradient(", repr(pointHigh), ")=", repr(
    ot.CenteredFiniteDifferenceGradient(1.0e-5, evaluation).gradient(pointHigh)))
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())

