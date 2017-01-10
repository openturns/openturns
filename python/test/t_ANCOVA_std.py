#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:

    RandomGenerator.SetSeed(0)

    # Problem parameters
    inputDimension = 2
    outputDimension = 1
    rho = 0.3
    a = 4.
    b = 5.

    # Reference analytical values
    covTh = a * a + b * b + 2 * a * b * rho
    Si = [[(a * a + a * b * rho) / covTh, a * a / covTh],
          [(b * b + a * b * rho) / covTh, b * b / covTh]]

    # Model
    inputName = ["X1", "X2", "a", "b"]
    formula = ["a * X1 + b * X2"]

    full = AnalyticalFunction(inputName, formula)
    model = ParametricFunction(full, [2, 3], [a, b])

    # Input distribution
    distribution = ComposedDistribution([Normal()] * inputDimension)

    # Correlated input distribution
    S = CorrelationMatrix(inputDimension)
    S[1, 0] = 0.3
    R = NormalCopula().GetCorrelationFromSpearmanCorrelation(S)
    myCopula = NormalCopula(R)
    myCorrelatedInputDistribution = ComposedDistribution(
        [Normal()] * inputDimension, myCopula)

    sample = myCorrelatedInputDistribution.getSample(2000)

    # Orthogonal basis
    enumerateFunction = EnumerateFunction(inputDimension)
    productBasis = OrthogonalProductPolynomialFactory(
        [HermiteFactory()] * inputDimension, enumerateFunction)
    # Adaptive strategy
    adaptiveStrategy = FixedStrategy(
        productBasis, enumerateFunction.getStrataCumulatedCardinal(4))
    # Projection strategy
    samplingSize = 250
    projectionStrategy = LeastSquaresStrategy(
        MonteCarloExperiment(samplingSize))

    # Polynomial chaos algorithm
    algo = FunctionalChaosAlgorithm(
        model, distribution, adaptiveStrategy, projectionStrategy)
    algo.run()

    # Post-process the results
    result = FunctionalChaosResult(algo.getResult())
    ancova = ANCOVA(result, sample)
    indices = ancova.getIndices()
    uncorrelatedIndices = ancova.getUncorrelatedIndices()

    for i in range(inputDimension):
        value = indices[i]
        print("ANCOVA index", i, "= %.8f" %
              value, "absolute error=%.10f" % fabs(value - Si[i][0]))
        value = uncorrelatedIndices[i]
        print("ANCOVA uncorrelated index", i, "= %.8f" %
              value, "absolute error=%.10f" % fabs(value - Si[i][1]))

except:
    import sys
    print("t_ANCOVA_std.py", sys.exc_info()[0], sys.exc_info()[1])
