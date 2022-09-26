#! /usr/bin/env python

import openturns as ot
from math import pi

ot.TESTPREAMBLE()

# Log.Show( Log.Flags() | Log.INFO )

# Problem parameters
dimension = 3
a = 7.0
b = 0.1
# Reference analytical values
meanTh = a / 2
covTh = (b ** 2 * pi ** 8) / 18.0 + \
    (b * pi ** 4) / 5.0 + (a ** 2) / 8.0 + 1.0 / 2.0
sob_1 = [(b * pi ** 4 / 5.0 + b ** 2 * pi ** 8 / 50.0 + 1.0 / 2.0)
          / covTh, (a ** 2 / 8.0) / covTh, 0.0]
sob_2 = [
    0.0, (b ** 2 * pi ** 8 / 18.0 - b ** 2 * pi ** 8 / 50.0) / covTh, 0.0]
sob_3 = [0.0]
sob_T1 = [sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0], sob_1[1] + sob_2[0]
          + sob_2[2] + sob_3[0], sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0]]
sob_T2 = [sob_2[0] + sob_2[1] + sob_3[0], sob_2[0]
          + sob_2[2] + sob_3[0], sob_2[1] + sob_2[2] + sob_3[0]]
sob_T3 = [sob_3[0]]
# Create the Ishigami function
inputVariables = ["xi1", "xi2", "xi3"]
formula = ["sin(xi1) + (" + str(a) + \
    ") * (sin(xi2)) ^ 2 + (" + str(
        b) + ") * xi3^4 * sin(xi1)"]
model = ot.SymbolicFunction(inputVariables, formula)

# Create the input distribution
distribution = ot.ComposedDistribution([ot.Uniform(-pi, pi)] * dimension)

# Create the orthogonal basis
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * dimension, enumerateFunction)

# design experiment
samplingSize = 75

# build basis
degree = 10
basisSize = enumerateFunction.getStrataCumulatedCardinal(degree)

# run algorithm
lar = ot.LARS()

listFittingAlgorithm = list()
listFittingAlgorithm.append(ot.KFold())
listFittingAlgorithm.append(ot.CorrectedLeaveOneOut())

for fittingAlgorithmIndex in range(len(listFittingAlgorithm)):
    fittingAlgorithm = listFittingAlgorithm[fittingAlgorithmIndex]
    adaptiveStrategy = ot.FixedStrategy(productBasis, basisSize)
    projectionStrategy = ot.LeastSquaresStrategy(ot.LeastSquaresMetaModelSelectionFactory(ot.LARS(), fittingAlgorithm))
    experiment = ot.LowDiscrepancyExperiment(ot.SobolSequence(), distribution, samplingSize)
    ot.RandomGenerator.SetSeed(0)
    X = experiment.generate()
    Y = model(X)
    algo = ot.FunctionalChaosAlgorithm(
        X, Y, distribution, adaptiveStrategy, projectionStrategy)
    algo.run()

    result = algo.getResult()
    print("coeffs = ", result.getCoefficients())
    print("residuals = ", result.getResiduals())
    print("relative errors = ", result.getRelativeErrors())

