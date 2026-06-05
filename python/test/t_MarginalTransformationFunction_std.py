#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp

ot.TESTPREAMBLE()

# Create input and output marginal distribution collections
inputColl = [ot.Normal(1.0, 2.5), ot.Gamma(1.5, 3.0, 0.0)]
outputColl = [ot.Gamma(2.5, 2.0, 0.0), ot.Normal(3.0, 1.5)]

# Build the MarginalTransformationFunction
f = otexp.MarginalTransformationFunction(inputColl, outputColl)
print("f=", f)

# Evaluate at a point
point = ot.Point([inputColl[0].computeQuantile(0.25)[0], inputColl[1].computeQuantile(0.25)[0]])
result = f(point)
print("f(", point, ")=", result)

# Check dimensions
assert f.getInputDimension() == 2, "wrong input dimension"
assert f.getOutputDimension() == 2, "wrong output dimension"
print("input dimension=", f.getInputDimension())
print("output dimension=", f.getOutputDimension())

# Check collections accessors
inputCollRef = f.getInputDistributionCollection()
outputCollRef = f.getOutputDistributionCollection()
assert len(inputCollRef) == len(inputColl), "wrong input distribution collection size"
assert len(outputCollRef) == len(outputColl), "wrong output distribution collection size"
for i in range(len(inputColl)):
    assert inputCollRef[i].getName() == inputColl[i].getName(), "wrong input distribution name"
    assert inputCollRef[i].getParameter() == inputColl[i].getParameter(), "wrong input distribution parameter"
for i in range(len(outputColl)):
    assert outputCollRef[i].getName() == outputColl[i].getName(), "wrong output distribution name"
    assert outputCollRef[i].getParameter() == outputColl[i].getParameter(), "wrong output distribution parameter"
print("input distribution collection=", inputCollRef)
print("output distribution collection=", outputCollRef)

# Check evaluation
assert result.getDimension() == 2, "wrong result dimension"
assert abs(result[0] - 0.668651) < 1e-5, "wrong first marginal"
assert abs(result[1] - 1.98827) < 1e-5, "wrong second marginal"
