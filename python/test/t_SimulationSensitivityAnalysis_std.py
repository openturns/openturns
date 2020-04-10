#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

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
comparisonOperators = [ot.Less(), ot.LessOrEqual(),
                       ot.Greater(), ot.GreaterOrEqual()]
threshold = 3.0
ot.ResourceMap.SetAsUnsignedInteger(
    "SimulationSensitivityAnalysis-DefaultSampleMargin", 10)
for i in range(4):
    # Analysis based on an event
    X = ot.RandomVector(distribution)
    Y = ot.CompositeRandomVector(f, X)
    event = ot.ThresholdEvent(Y, comparisonOperators[i], threshold)
    algo = ot.SimulationSensitivityAnalysis(event, inputSample, outputSample)
    print("algo=", algo)
    # Perform the analysis
    print("Mean point in event domain=",
          algo.computeMeanPointInEventDomain())
    print("Importance factors at threshold ", threshold,
          " =", algo.computeImportanceFactors())
    print("Importance factors at threshold/2 ", threshold / 2,
          " =", algo.computeImportanceFactors(threshold / 2))
    importanceFactorsGraph = algo.drawImportanceFactors()
    print("importanceFactorsGraph=", importanceFactorsGraph)

    # Importance factors evolution on probability scale
    importanceFactorsRangeGraphProbability = algo.drawImportanceFactorsRange(
    )
    print("importanceFactorsRangeGraphProbability=",
          importanceFactorsRangeGraphProbability)

    # Importance factors evolution on threshold scale
    importanceFactorsRangeGraphThreshold = algo.drawImportanceFactorsRange(
        False)
    print("importanceFactorsRangeGraphThreshold=",
          importanceFactorsRangeGraphThreshold)
