#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
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

    # Create the adaptive strategy
    # We can choose amongst several strategies
    # First, the most efficient (but more complex!) strategy
    listAdaptiveStrategy = list()
    degree = 6
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
        # We have only the LeastSquaresStrategy up to now (0.13.0) but we can choose several sampling schemes
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
            pGrad = result.getMetaModel().parameterGradient(
                distribution.getMean())
            print("###################################")
            print(algo.getAdaptiveStrategy())
            print(algo.getProjectionStrategy())
            # print "coefficients=", result.getCoefficients()
            residuals = result.getResiduals()
            print("residuals=", residuals)
            relativeErrors = result.getRelativeErrors()
            print("relativeErrors=", relativeErrors)

            # Post-process the results
            vector = FunctionalChaosRandomVector(result)
            mean = vector.getMean()[0]
            print("mean=%.8f" % mean, "absolute error=%.10f" %
                  fabs(mean - meanTh))
            variance = vector.getCovariance()[0, 0]
            print("variance=%.8f" % variance, "absolute error=%.10f" %
                  fabs(variance - covTh))
            sensitivity = FunctionalChaosSobolIndices(result)
            for i in range(dimension):
                value = sensitivity.getSobolIndex(i)
                print("Sobol index", i, "= %.8f" %
                      value, "absolute error=%.10f" % fabs(value - sob_1[i]))
            indices = Indices(2)
            k = 0
            for i in range(dimension):
                indices[0] = i
                for j in range(i + 1, dimension):
                    indices[1] = j
                    value = sensitivity.getSobolIndex(indices)
                    print("Sobol index", indices, "=%.8f" %
                          value, "absolute error=%.10f" % fabs(value - sob_2[k]))
                    k = k + 1
            indices = Indices(3)
            indices[0] = 0
            indices[1] = 1
            indices[2] = 2
            value = sensitivity.getSobolIndex(indices)
            print("Sobol index", indices, "=%.8f" %
                  value, "absolute error=%.10f" % fabs(value - sob_3[0]))
            for i in range(dimension):
                value = sensitivity.getSobolTotalIndex(i)
                print("Sobol total index", i, "=%.8f" %
                      value, "absolute error=%.10f" % fabs(value - sob_T1[i]))
            indices = Indices(2)
            k = 0
            for i in range(dimension):
                indices[0] = i
                for j in range(i + 1, dimension):
                    indices[1] = j
                    value = sensitivity.getSobolIndex(indices)
                    print("Sobol total index", indices, "=%.8f" %
                          value, "absolute error=%.10f" % fabs(value - sob_2[k]))
                    k = k + 1
            indices = Indices(3)
            indices[0] = 0
            indices[1] = 1
            indices[2] = 2
            value = sensitivity.getSobolTotalIndex(indices)
            print("Sobol total index ", indices, "=%.8f" %
                  value, "absolute error=%.10f" % fabs(value - sob_3[0]))
            for i in range(dimension):
                value = sensitivity.getSobolGroupedIndex(i)
                print("Sobol grouped index", i, "=%.8f" %
                      value, "absolute error=%.10f" % fabs(value - sob_T1[i]))
            indices = Indices(2)
            k = 0
            for i in range(dimension):
                indices[0] = i
                for j in range(i + 1, dimension):
                    indices[1] = j
                    value = sensitivity.getSobolGroupedIndex(indices)
                    print("Sobol grouped index", indices, "=%.8f" %
                          value, "absolute error=%.10f" % fabs(value - sob_2[k]))
                    k = k + 1
            indices = Indices(3)
            indices[0] = 0
            indices[1] = 1
            indices[2] = 2
            value = sensitivity.getSobolGroupedIndex(indices)
            print("Sobol grouped index ", indices, "=%.8f" %
                  value, "absolute error=%.10f" % fabs(value - sob_3[0]))
            indices = Indices(3)
            indices[0] = 0
            indices[1] = 1
            indices[2] = 2
            value = sensitivity.getSobolGroupedTotalIndex(indices)
            print("Sobol grouped total index ", indices, "=%.8f" %
                  value, "absolute error=%.10f" % fabs(value - 1.))

except:
    import sys
    print("t_FunctionalChaos_ishigami.py",
          sys.exc_info()[0], sys.exc_info()[1])
