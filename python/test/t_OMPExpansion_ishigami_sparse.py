#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
from math import pi

ot.TESTPREAMBLE()

# Problem parameters
dimension = 3
a = 7.0
b = 0.1
# Reference analytical values
covTh = (b**2 * pi**8) / 18.0 + (b * pi**4) / 5.0 + (a**2) / 8.0 + 1.0 / 2.0
sob_1 = [
    (b * pi**4 / 5.0 + b**2 * pi**8 / 50.0 + 1.0 / 2.0) / covTh,
    (a**2 / 8.0) / covTh,
    0.0,
]
sob_2 = [0.0, (b**2 * pi**8 / 18.0 - b**2 * pi**8 / 50.0) / covTh, 0.0]
sob_3 = [0.0]
sob_T1 = [
    sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0],
    sob_1[1] + sob_2[0] + sob_2[2] + sob_3[0],
    sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0],
]

# Create the Ishigami function
inputVariables = ["xi1", "xi2", "xi3"]
formula = [
    "sin(xi1) + (" + str(a) + ") * (sin(xi2)) ^ 2 + (" + str(b) + ") * xi3^4 * sin(xi1)"
]
model = ot.SymbolicFunction(inputVariables, formula)

# Create the input distribution
distribution = ot.JointDistribution([ot.Uniform(-pi, pi)] * dimension)

# Create the orthogonal basis
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * dimension, enumerateFunction
)

# Design experiment
samplingSize = 75
ot.RandomGenerator.SetSeed(0)
inputSample = distribution.getSample(samplingSize)
outputSample = model(inputSample)

# Build basis
degree = 10
basisSize = enumerateFunction.getStrataCumulatedCardinal(degree)

# Run algorithm with two fitting algorithms, matching the reference
# FunctionalChaos Ishigami sparse test structure
listFittingAlgorithm = [ot.KFold(), ot.CorrectedLeaveOneOut()]

for fittingAlgorithm in listFittingAlgorithm:
    algo = otexp.OMPExpansion(
        inputSample, outputSample, distribution, productBasis, basisSize, "QR", fittingAlgorithm
    )
    algo.run()
    result = algo.getResult()
    print("fitting=", fittingAlgorithm.getClassName())
    print("coeffs=", result.getCoefficients())
    print("isLeastSquares=", result.isLeastSquares())
    assert result.isLeastSquares()
    print("involvesModelSelection=", result.involvesModelSelection())
    assert result.involvesModelSelection()

    # Check Sobol indices
    sobolIndices = ot.FunctionalChaosSobolIndices(result)
    for i in range(dimension):
        s1 = sobolIndices.getSobolIndex(i)
        st = sobolIndices.getSobolTotalIndex(i)
        print("  X", i + 1, ": S1 ref=", sob_1[i], " computed=", s1)
        ott.assert_almost_equal(s1, sob_1[i], 1e-2, 1e-2)
        print("  X", i + 1, ": ST ref=", sob_T1[i], " computed=", st)
        ott.assert_almost_equal(st, sob_T1[i], 1e-2, 1e-2)
    print()
