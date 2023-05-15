#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()

# Problem parameters
dimension = 3
a = 7.0
b = 0.1
# Reference analytical values
meanTh = a / 2
covTh = (b ** 2 * m.pi ** 8) / 18.0 + (b * m.pi ** 4) / 5.0 + (a ** 2) / 8.0 + 1.0 / 2.0
sob_1 = [
    (b * m.pi ** 4 / 5.0 + b ** 2 * m.pi ** 8 / 50.0 + 1.0 / 2.0) / covTh,
    (a ** 2 / 8.0) / covTh,
    0.0,
]
sob_2 = [0.0, (b ** 2 * m.pi ** 8 / 18.0 - b ** 2 * m.pi ** 8 / 50.0) / covTh, 0.0]
sob_3 = [0.0]
sob_T1 = [
    sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0],
    sob_1[1] + sob_2[0] + sob_2[2] + sob_3[0],
    sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0],
]
sob_T2 = [
    sob_2[0] + sob_2[1] + sob_3[0],
    sob_2[0] + sob_2[2] + sob_3[0],
    sob_2[1] + sob_2[2] + sob_3[0],
]
sob_T3 = [sob_3[0]]
# Create the Ishigami function
inputVariables = ["xi1", "xi2", "xi3"]
formula = ot.Description(1)
formula[0] = (
    "sin(xi1) + (" + str(a) + ") * (sin(xi2)) ^ 2 + (" + str(b) + ") * xi3^4 * sin(xi1)"
)
model = ot.SymbolicFunction(inputVariables, formula)

# Create the input distribution
distribution = ot.ComposedDistribution([ot.Uniform(-m.pi, m.pi)] * dimension)

# Create the orthogonal basis
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * dimension, enumerateFunction
)

# Create the adaptive strategy
# We can choose amongst several strategies
# First, the most efficient (but more complex!) strategy
listAdaptiveStrategy = list()
degree = 6
indexMax = enumerateFunction.getStrataCumulatedCardinal(degree)
basisDimension = enumerateFunction.getStrataCumulatedCardinal(degree // 2)
threshold = 1.0e-6
listAdaptiveStrategy.append(
    ot.CleaningStrategy(productBasis, indexMax, basisDimension, threshold, False)
)
# Second, the most used (and most basic!) strategy
listAdaptiveStrategy.append(
    ot.FixedStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree))
)

for adaptiveStrategyIndex in range(len(listAdaptiveStrategy)):
    adaptiveStrategy = listAdaptiveStrategy[adaptiveStrategyIndex]
    # Create the projection strategy
    samplingSize = 250
    listExperiment = list()
    # We have only the LeastSquaresStrategy up to now (0.13.0) but we can choose several sampling schemes
    # Monte Carlo sampling
    listExperiment.append(ot.MonteCarloExperiment(distribution, samplingSize))
    # LHS sampling
    listExperiment.append(ot.LHSExperiment(distribution, samplingSize))
    # Low Discrepancy sequence
    listExperiment.append(
        ot.LowDiscrepancyExperiment(ot.SobolSequence(), distribution, samplingSize)
    )
    for experiment in listExperiment:
        # Create the polynomial chaos algorithm
        maximumResidual = 1.0e-10
        ot.RandomGenerator.SetSeed(0)
        X = experiment.generate()
        Y = model(X)
        algo = ot.FunctionalChaosAlgorithm(X, Y, distribution, adaptiveStrategy)
        algo.setMaximumResidual(maximumResidual)
        algo.run()

        # Examine the results
        result = ot.FunctionalChaosResult(algo.getResult())
        pGrad = result.getMetaModel().parameterGradient(distribution.getMean())
        print("###################################")
        print(algo.getAdaptiveStrategy())
        print(algo.getProjectionStrategy())
        # print "coefficients=", result.getCoefficients()
        residuals = result.getResiduals()
        print("residuals=", residuals)
        relativeErrors = result.getRelativeErrors()
        print("relativeErrors=", relativeErrors)

        # Post-process the results
        vector = ot.FunctionalChaosRandomVector(result)
        mean = vector.getMean()[0]
        print("mean=%.8f" % mean, "absolute error=%.10f" % abs(mean - meanTh))
        variance = vector.getCovariance()[0, 0]
        print(
            "variance=%.8f" % variance, "absolute error=%.10f" % abs(variance - covTh)
        )
        sensitivity = ot.FunctionalChaosSobolIndices(result)
        for i in range(dimension):
            value = sensitivity.getSobolIndex(i)
            print(
                "Sobol index",
                i,
                "= %.8f" % value,
                "absolute error=%.10f" % abs(value - sob_1[i]),
            )
        indices = ot.Indices(2)
        k = 0
        for i in range(dimension):
            indices[0] = i
            for j in range(i + 1, dimension):
                indices[1] = j
                value = sensitivity.getSobolIndex(indices)
                print(
                    "Sobol index",
                    indices,
                    "=%.8f" % value,
                    "absolute error=%.10f" % abs(value - sob_2[k]),
                )
                k = k + 1
        indices = ot.Indices([0, 1, 2])
        value = sensitivity.getSobolIndex(indices)
        print(
            "Sobol index",
            indices,
            "=%.8f" % value,
            "absolute error=%.10f" % abs(value - sob_3[0]),
        )
        for i in range(dimension):
            value = sensitivity.getSobolTotalIndex(i)
            print(
                "Sobol total index",
                i,
                "=%.8f" % value,
                "absolute error=%.10f" % abs(value - sob_T1[i]),
            )
        indices = ot.Indices(2)
        k = 0
        for i in range(dimension):
            indices[0] = i
            for j in range(i + 1, dimension):
                indices[1] = j
                value = sensitivity.getSobolIndex(indices)
                print(
                    "Sobol total index",
                    indices,
                    "=%.8f" % value,
                    "absolute error=%.10f" % abs(value - sob_2[k]),
                )
                k = k + 1
        indices = ot.Indices([0, 1, 2])
        value = sensitivity.getSobolTotalIndex(indices)
        print(
            "Sobol total index ",
            indices,
            "=%.8f" % value,
            "absolute error=%.10f" % abs(value - sob_3[0]),
        )
        indices = ot.Indices(2)
        k = 0
        for i in range(dimension):
            indices[0] = i
            for j in range(i + 1, dimension):
                indices[1] = j
                value = sensitivity.getSobolGroupedIndex(indices)
                print(
                    "Sobol grouped index",
                    indices,
                    "=%.8f" % value,
                    "absolute error=%.10f" % abs(value - sob_2[k]),
                )
                k = k + 1
        indices = ot.Indices([0, 1, 2])
        value = sensitivity.getSobolGroupedIndex(indices)
        print(
            "Sobol grouped index ",
            indices,
            "=%.8f" % value,
            "absolute error=%.10f" % abs(value - sob_3[0]),
        )
        indices = ot.Indices([0, 1, 2])
        value = sensitivity.getSobolGroupedTotalIndex(indices)
        print(
            "Sobol grouped total index ",
            indices,
            "=%.8f" % value,
            "absolute error=%.10f" % abs(value - 1.0),
        )
        # Get part of variance indices
        print("Part of variance")
        partOfVariance = sensitivity.getPartOfVariance()
        result = sensitivity.getFunctionalChaosResult()
        orthogonalBasis = result.getOrthogonalBasis()
        enumerateFunction = orthogonalBasis.getEnumerateFunction()
        indices = result.getIndices()
        basisSize = indices.getSize()
        for i in range(basisSize):
            globalIndex = indices[i]
            multiIndex = enumerateFunction(globalIndex)
            if partOfVariance[i] > 1.0e-3:
                print(
                    "%d, %d, %s, %.4f" % (i, globalIndex, multiIndex, partOfVariance[i])
                )
        # Print summary
        print(sensitivity)
