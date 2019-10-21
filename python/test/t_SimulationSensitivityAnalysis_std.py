#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Uncertain parameters
    distribution = Normal(
        Point(3, 1.0), Point(3, 2.0), CorrelationMatrix(3))
    distribution.setName("Unnamed")
    # Model
    inputVar = Description(["x", "y", "z"])
    formulas = Description(["x-1.5*y+2*z"])
    f = SymbolicFunction(inputVar, formulas)
    # Must activate the history mechanism if one want to perform sensitivity
    # analysis
    f = MemoizeFunction(f)
    # Sampling
    size = 100
    inputSample = distribution.getSample(size)
    outputSample = f(inputSample)
    comparisonOperators = [Less(), LessOrEqual(), Greater(), GreaterOrEqual()]
    threshold = 3.0
    ResourceMap.SetAsUnsignedInteger(
        "SimulationSensitivityAnalysis-DefaultSampleMargin", 10)
    for i in range(4):
        algo = SimulationSensitivityAnalysis(
            inputSample, outputSample, distribution.getIsoProbabilisticTransformation(), comparisonOperators[i], threshold)
        print("algo=", algo)
        # Perform the analysis
        print("Mean point in event domain=",
              algo.computeMeanPointInEventDomain())
        print("Importance factors at ", threshold,
              " =", algo.computeImportanceFactors())
        print("Importance factors at ", threshold / 2, " =",
              algo.computeImportanceFactors(threshold / 2))
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

        # Reset the history of the model
        f.clearHistory()

        # Analysis based on an event
        X = RandomVector(distribution)
        Y = CompositeRandomVector(f, X)
        event = ThresholdEvent(Y, comparisonOperators[i], threshold)
        # Get a sample of the event to simulate a Monte Carlo analysis. We
        # don't care of the result as the interesting values are stored in the
        # model history
        event.getSample(size)
        algo = SimulationSensitivityAnalysis(event)
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

except:
    import sys
    print("t_SimulationSensitivityAnalysis_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
