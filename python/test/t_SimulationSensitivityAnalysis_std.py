#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Uncertain parameters
distribution = ot.Normal([1.0] * 3, [2.0] * 3, ot.CorrelationMatrix(3))
distribution.setName("Unnamed")
# Model
f = ot.SymbolicFunction(["x", "y", "z"], ["x-1.5*y+2*z"])
# Sampling
size = 100
inputSample = distribution.getSample(size)
outputSample = f(inputSample)
comparisonOperators = [ot.Less(), ot.LessOrEqual(), ot.Greater(), ot.GreaterOrEqual()]
threshold = 3.0
ot.ResourceMap.SetAsUnsignedInteger(
    "SimulationSensitivityAnalysis-DefaultSampleMargin", 10
)

# Reference values obtained from a run of this test
meanPoints = [
    [0.6096864303268671, 1.6493974966960445, 0.21546002016713695],
    [0.6096864303268671, 1.6493974966960445, 0.21546002016713695],
    [1.5457374650118587, -0.5465105290648214, 2.3583928126728537],
    [1.5457374650118587, -0.5465105290648214, 2.3583928126728537],
]
importanceFactors = [
    [0.12806758096836415, 0.35451378426351093, 0.517418634768125],
    [0.12806758096836415, 0.35451378426351093, 0.517418634768125],
    [0.06567705764981199, 0.5274143134103128, 0.4069086289398753],
    [0.06567705764981199, 0.5274143134103128, 0.4069086289398753],
]
importanceFactorsHalf = [
    [0.2153651807348037, 0.22624291640893246, 0.5583919028562639],
    [0.2153651807348037, 0.22624291640893246, 0.5583919028562639],
    [0.14301806883790266, 0.38131119750308023, 0.475670733659017],
    [0.14301806883790266, 0.38131119750308023, 0.475670733659017],
]

for i in range(4):
    # Analysis based on an event
    X = ot.RandomVector(distribution)
    Y = ot.CompositeRandomVector(f, X)
    event = ot.ThresholdEvent(Y, comparisonOperators[i], threshold)
    algo = ot.SimulationSensitivityAnalysis(event, inputSample, outputSample)
    assert algo.getInputSample().getSize() == size
    assert algo.getInputSample().getDimension() == 3
    assert algo.getOutputSample().getSize() == size
    # Perform the analysis
    ott.assert_almost_equal(
        algo.computeMeanPointInEventDomain(), meanPoints[i], 1e-6, 1e-6
    )
    importanceFactorsAtThreshold = algo.computeImportanceFactors()
    ott.assert_almost_equal(
        importanceFactorsAtThreshold, importanceFactors[i], 1e-6, 1e-6
    )
    assert abs(sum(importanceFactorsAtThreshold) - 1.0) < 1e-6
    assert all(0.0 <= v <= 1.0 for v in importanceFactorsAtThreshold)
    importanceFactorsAtHalf = algo.computeImportanceFactors(threshold / 2)
    ott.assert_almost_equal(
        importanceFactorsAtHalf, importanceFactorsHalf[i], 1e-6, 1e-6
    )
    assert abs(sum(importanceFactorsAtHalf) - 1.0) < 1e-6
    assert all(0.0 <= v <= 1.0 for v in importanceFactorsAtHalf)

    importanceFactorsGraph = algo.drawImportanceFactors()
    assert isinstance(importanceFactorsGraph, ot.Graph)
    assert "Importance" in importanceFactorsGraph.getTitle()
    assert len(importanceFactorsGraph.getDrawables()) >= 1

    # Importance factors evolution on probability scale
    importanceFactorsRangeGraphProbability = algo.drawImportanceFactorsRange()
    assert isinstance(importanceFactorsRangeGraphProbability, ot.Graph)
    assert "Importance" in importanceFactorsRangeGraphProbability.getTitle()
    assert len(importanceFactorsRangeGraphProbability.getDrawables()) >= 1

    # Importance factors evolution on threshold scale
    importanceFactorsRangeGraphThreshold = algo.drawImportanceFactorsRange(False)
    assert isinstance(importanceFactorsRangeGraphThreshold, ot.Graph)
    assert "Importance" in importanceFactorsRangeGraphThreshold.getTitle()
    assert len(importanceFactorsRangeGraphThreshold.getDrawables()) >= 1
