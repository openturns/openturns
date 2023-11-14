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
covTh = (b**2 * m.pi**8) / 18.0 + (b * m.pi**4) / 5.0 + (a**2) / 8.0 + 1.0 / 2.0
sob_1 = [
    (b * m.pi**4 / 5.0 + b**2 * m.pi**8 / 50.0 + 1.0 / 2.0) / covTh,
    (a**2 / 8.0) / covTh,
    0.0,
]
sob_2 = [0.0, (b**2 * m.pi**8 / 18.0 - b**2 * m.pi**8 / 50.0) / covTh, 0.0]
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

# Create the projection strategy
samplingSize = 250
listProjectionStrategy = list()
# Monte Carlo sampling
inputSample = ot.LowDiscrepancyExperiment(
    ot.SobolSequence(), distribution, samplingSize
).generate()
outputSample = model(inputSample)
# From here, the model is no more needed
listProjectionStrategy.append(ot.LeastSquaresStrategy())
listProjectionStrategy.append(
    ot.LeastSquaresStrategy(
        ot.LeastSquaresMetaModelSelectionFactory(ot.LARS(), ot.CorrectedLeaveOneOut())
    )
)
listProjectionStrategy.append(ot.IntegrationStrategy())
# Create the adaptive strategy
# We can choose amongst several strategies
# First, the most efficient (but more complex!) strategy
degree = 6
listAdaptiveStrategy = list()
indexMax = enumerateFunction.getStrataCumulatedCardinal(degree)
basisDimension = enumerateFunction.getStrataCumulatedCardinal(degree // 2)
threshold = 1.0e-6
listAdaptiveStrategy.append(
    ot.CleaningStrategy(productBasis, indexMax, basisDimension, threshold)
)
# Second, the most used (and most basic!) strategy
listAdaptiveStrategy.append(
    ot.FixedStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree))
)

for adaptiveStrategyIndex in range(len(listAdaptiveStrategy)):
    adaptiveStrategy = listAdaptiveStrategy[adaptiveStrategyIndex]
    for projectionStrategyIndex in range(len(listProjectionStrategy)):
        projectionStrategy = listProjectionStrategy[projectionStrategyIndex]
        # Create the polynomial chaos algorithm
        maximumResidual = 1.0e-10
        algo = ot.FunctionalChaosAlgorithm(
            inputSample,
            outputSample,
            distribution,
            adaptiveStrategy,
            projectionStrategy,
        )
        algo.setMaximumResidual(maximumResidual)
        ot.RandomGenerator.SetSeed(0)
        algo.run()

        # Examine the results
        result = algo.getResult()
        print("###################################")
        print(adaptiveStrategy)
        print(algo.getProjectionStrategy())
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
