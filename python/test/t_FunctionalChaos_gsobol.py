#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)


def sobol(indice, ai):
    val = 1.0
    for i in range(indice.getSize()):
        val = val * 1.0 / (3.0 * (1.0 + ai[indice[i]]) ** 2.0)
    return val


try:
    # Problem parameters
    dimension = 3

    # Create the Sobol function
    # Reference analytical values
    meanTh = 1.0
    a = Point(dimension)
    inputVariables = Description(dimension)
    formula = Description(1)
    formula[0] = "1.0"
    covTh = 1.0
    for i in range(dimension):
        a[i] = 0.5 * i
        covTh = covTh * (1.0 + 1.0 / (3.0 * (1.0 + a[i]) ** 2))
        inputVariables[i] = "xi" + str(i)
        formula[0] = formula[0] + \
            " * ((abs(4.0 * xi" + str(i) + " - 2.0) + " + \
            str(a[i]) + ") / (1.0 + " + str(a[i]) + "))"
    covTh = covTh - 1.0
    model = SymbolicFunction(inputVariables, formula)

    # Create the input distribution
    marginals = [Uniform(0.0, 1.0)] * dimension
    distribution = ComposedDistribution(marginals)

    # Create the orthogonal basis
    enumerateFunction = LinearEnumerateFunction(dimension)
    productBasis = OrthogonalProductPolynomialFactory(marginals)

    # Create the adaptive strategy
    # We can choose amongst several strategies
    # First, the most efficient (but more complex!) strategy
    listAdaptiveStrategy = list()
    degree = 4
    indexMax = enumerateFunction.getStrataCumulatedCardinal(degree)
    basisDimension = enumerateFunction.getStrataCumulatedCardinal(degree // 2)
    threshold = 1.0e-6
    listAdaptiveStrategy.append(
        CleaningStrategy(productBasis, indexMax, basisDimension, threshold, False))
    # Second, the most used (and most basic!) strategy
    listAdaptiveStrategy.append(
        FixedStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree)))
    # Third, a slight enhancement with respect to the basic strategy
    listAdaptiveStrategy.append(
        SequentialStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree // 2), False))

    for adaptiveStrategyIndex in range(len(listAdaptiveStrategy)):
        adaptiveStrategy = listAdaptiveStrategy[adaptiveStrategyIndex]
        # Create the projection strategy
        samplingSize = 250
        listProjectionStrategy = list()
        # Monte Carlo sampling
        listProjectionStrategy.append(
            LeastSquaresStrategy(MonteCarloExperiment(samplingSize)))
        # LHS sampling
        listProjectionStrategy.append(
            LeastSquaresStrategy(LHSExperiment(samplingSize)))
        # Low Discrepancy sequence
        listProjectionStrategy.append(LeastSquaresStrategy(
            LowDiscrepancyExperiment(SobolSequence(), samplingSize)))
        for projectionStrategyIndex in range(len(listProjectionStrategy)):
            projectionStrategy = listProjectionStrategy[
                projectionStrategyIndex]
            # Create the polynomial chaos algorithm
            maximumResidual = 1.0e-10
            algo = FunctionalChaosAlgorithm(
                model, distribution, adaptiveStrategy, projectionStrategy)
            algo.setMaximumResidual(maximumResidual)
            RandomGenerator.SetSeed(0)
            algo.run()

            # Examine the results
            result = FunctionalChaosResult(algo.getResult())
            print("###################################")
            print(algo.getAdaptiveStrategy())
            print(algo.getProjectionStrategy())
            # print "coefficients=", result.getCoefficients()
            residuals = result.getResiduals()
            print("residuals=", residuals)
            relativeErrors = result.getRelativeErrors()
            print("relative errors=", relativeErrors)

            # Post-process the results
            vector = FunctionalChaosRandomVector(result)
            mean = vector.getMean()[0]
            print("mean=%.8f" % mean, "absolute error=%.8f" %
                  fabs(mean - meanTh))
            variance = vector.getCovariance()[0, 0]
            sensitivity = FunctionalChaosSobolIndices(result)
            print("variance=%.8f" % variance, "absolute error=%.8f" %
                  fabs(variance - covTh))
            indices = Indices(1)
            for i in range(dimension):
                indices[0] = i
                value = sensitivity.getSobolIndex(i)
                print("Sobol index", i, "= %.8f" % value, "absolute error=%.8f" %
                      fabs(value - sobol(indices, a) / covTh))
            indices = Indices(2)
            k = 0
            for i in range(dimension):
                indices[0] = i
                for j in range(i + 1, dimension):
                    indices[1] = j
                    value = sensitivity.getSobolIndex(indices)
                    print("Sobol index", indices, "=%.8f" % value, "absolute error=%.8f" % fabs(
                        value - sobol(indices, a) / covTh))
                    k = k + 1
            indices = Indices(3)
            indices[0] = 0
            indices[1] = 1
            indices[2] = 2
            value = sensitivity.getSobolIndex(indices)
            print("Sobol index", indices, "=%.8f" % value, "absolute error=%.8f" %
                  fabs(value - sobol(indices, a) / covTh))

except:
    import sys
    print("t_FunctionalChaos_gsobol.py", sys.exc_info()[0], sys.exc_info()[1])
