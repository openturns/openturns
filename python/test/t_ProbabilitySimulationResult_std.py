#! /usr/bin/env python

import os
import openturns as ot
from openturns.testing import assert_almost_equal, assert_raises

ot.TESTPREAMBLE()

# the handle keeps the dynamic type of the concrete result it wraps
X = ot.RandomVector(ot.Normal())
Y = ot.CompositeRandomVector(ot.SymbolicFunction(["X"], ["X"]), X)
event = ot.ThresholdEvent(Y, ot.Less(), 0.0)
implementation = ot.ProbabilitySimulationResultImplementation(
    event, 0.1, 0.01, 100, 10
)
result = ot.ProbabilitySimulationResult(implementation)

assert result.getImplementation().getClassName() == "ProbabilitySimulationResultImplementation"
assert_almost_equal(result.getProbabilityEstimate(), 0.1)
assert_almost_equal(result.getVarianceEstimate(), 0.01)
assert result.getOuterSampling() == 100
assert result.getBlockSize() == 10
assert_almost_equal(result.getTimeDuration(), 0.0)
assert result.getEvent().isEvent()
assert result.getEvent().getDescription() == event.getDescription()

# the handle owns a clone: changing the source implementation has no effect
implementation.setProbabilityEstimate(0.5)
assert_almost_equal(result.getProbabilityEstimate(), 0.1)

# getImplementation() returns a copy, not a way to mutate the handle
result.getImplementation().setProbabilityEstimate(0.9)
assert_almost_equal(result.getProbabilityEstimate(), 0.1)

# the variance setter applies the constructor validation: negative values
# and NaN are rejected instead of silently corrupting the estimate
validated = ot.ProbabilitySimulationResultImplementation(
    event, 0.1, 0.01, 100, 10
)
validated.setVarianceEstimate(0.25)
assert_almost_equal(validated.getVarianceEstimate(), 0.25)
with assert_raises(TypeError):
    validated.setVarianceEstimate(-0.1)
with assert_raises(TypeError):
    validated.setVarianceEstimate(float("nan"))
assert_almost_equal(validated.getVarianceEstimate(), 0.25)

# copying the handle shares the implementation until a setter writes: the
# copy-on-write must keep the original result unchanged
copy = ot.ProbabilitySimulationResult(result)
copy.setOuterSampling(7)
copy.setBlockSize(3)
copy.setTimeDuration(1.5)
assert copy.getOuterSampling() == 7
assert copy.getBlockSize() == 3
assert_almost_equal(copy.getTimeDuration(), 1.5)
assert result.getOuterSampling() == 100
assert result.getBlockSize() == 10
assert_almost_equal(result.getTimeDuration(), 0.0)

# the ResourceMap keys keep their original names and can still be set
key = "ProbabilitySimulationResult-DefaultConfidenceLevel"
oldValue = ot.ResourceMap.GetAsScalar(key)
ot.ResourceMap.SetAsScalar(key, 0.9)
assert_almost_equal(ot.ResourceMap.GetAsScalar(key), 0.9)
assert_almost_equal(result.getConfidenceLength(), result.getConfidenceLength(0.9))
ot.ResourceMap.SetAsScalar(key, oldValue)
assert_almost_equal(ot.ResourceMap.GetAsScalar(key), oldValue)

boolKey = "ProbabilitySimulationResult-CheckPositiveVariance"
oldBoolValue = ot.ResourceMap.GetAsBool(boolKey)
ot.ResourceMap.SetAsBool(boolKey, not oldBoolValue)
assert ot.ResourceMap.GetAsBool(boolKey) == (not oldBoolValue)
ot.ResourceMap.SetAsBool(boolKey, oldBoolValue)
assert ot.ResourceMap.GetAsBool(boolKey) == oldBoolValue

# a Study saved before the split stores the result under the former class
# name: it must still load as its implementation
try:
    fileName = "t_ProbabilitySimulationResult_legacy.xml"
    study = ot.Study(fileName)
    study.add("result", result)
    study.save()
    with open(fileName) as stream:
        content = stream.read()
    content = content.replace(
        "ProbabilitySimulationResultImplementation", "ProbabilitySimulationResult"
    )
    with open(fileName, "w") as stream:
        stream.write(content)
    study2 = ot.Study(fileName)
    study2.load()
    legacy = ot.ProbabilitySimulationResult()
    study2.fillObject("result", legacy)
    assert_almost_equal(legacy.getProbabilityEstimate(), 0.1)
    assert legacy.getOuterSampling() == 100
    os.remove(fileName)
except ot.NotYetImplementedException:
    pass

print("OK")
