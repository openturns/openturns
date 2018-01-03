#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:
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
    inputVariables = Description(dimension)
    inputVariables[0] = "xi1"
    inputVariables[1] = "xi2"
    inputVariables[2] = "xi3"
    outputVariables = Description(1)
    outputVariables[0] = "y"
    formula = Description(1)
    formula[0] = "sin(xi1) + (" + str(a) + \
        ") * (sin(xi2)) ^ 2 + (" + str(
            b) + ") * xi3^4 * sin(xi1)"
    model = SymbolicFunction(inputVariables, formula)

    # Create the input distribution
    distribution = ComposedDistribution([Uniform(-pi, pi)] * dimension)

    # Create the orthogonal basis
    enumerateFunction = LinearEnumerateFunction(dimension)
    productBasis = OrthogonalProductPolynomialFactory(
        [LegendreFactory()] * dimension, enumerateFunction)

    # design experiment
    samplingSize = 75

    # build basis
    degree = 10
    basisSize = enumerateFunction.getStrataCumulatedCardinal(degree)

    # run algorithm
    lar = LARS()

    listFittingAlgorithm = list()
    listFittingAlgorithm.append(KFold())
    listFittingAlgorithm.append(CorrectedLeaveOneOut())

    for fittingAlgorithmIndex in range(len(listFittingAlgorithm)):
        fittingAlgorithm = listFittingAlgorithm[fittingAlgorithmIndex]
        adaptiveStrategy = FixedStrategy(productBasis, basisSize)
        projectionStrategy = LeastSquaresStrategy(LowDiscrepancyExperiment(
            SobolSequence(dimension), samplingSize), LeastSquaresMetaModelSelectionFactory(LARS(), fittingAlgorithm))
        algo = FunctionalChaosAlgorithm(
            model, distribution, adaptiveStrategy, projectionStrategy)
        RandomGenerator.SetSeed(0)
        algo.run()

        result = algo.getResult()
        print("coeffs = ", result.getCoefficients())
        print("residuals = ", result.getResiduals())
        print("relative errors = ", result.getRelativeErrors())


except:
    import sys
    print("t_LeastSquaresMetaModelSelection_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
