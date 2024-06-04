#! /usr/bin/env python

import openturns as ot
from openturns.usecases import ishigami_function

ot.TESTPREAMBLE()

# Problem parameters
im = ishigami_function.IshigamiModel()
distribution = im.distributionX
model = im.model
dimension = im.distributionX.getDimension()
sobolFirstOrderReference = [im.S1, im.S2, im.S3]
sobolSecondOrderReference = [im.S12, im.S13, im.S23]
sobolTotalReference = [im.ST1, im.ST2, im.ST3]
sobolInteractionTotalReference2 = [im.S12 + im.S123, im.S13 + im.S123, im.S23 + im.S123]
sobolInteractionTotalReference3 = [im.S123]
sobolGroupedIndex2 = [
    im.S1 + im.S2 + im.S12,
    im.S1 + im.S3 + im.S13,
    im.S2 + im.S3 + im.S23,
]
sobolGroupedIndex3 = [im.S1 + im.S2 + im.S3 + im.S12 + im.S13 + im.S23 + im.S123]

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
indexMax = enumerateFunction.getBasisSizeFromTotalDegree(degree)
basisDimension = enumerateFunction.getBasisSizeFromTotalDegree(degree // 2)
threshold = 1.0e-6
listAdaptiveStrategy.append(
    ot.CleaningStrategy(productBasis, indexMax, basisDimension, threshold)
)
# Second, the most used (and most basic!) strategy
listAdaptiveStrategy.append(
    ot.FixedStrategy(
        productBasis, enumerateFunction.getBasisSizeFromTotalDegree(degree)
    )
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
        result = algo.getResult()
        pGrad = result.getMetaModel().parameterGradient(distribution.getMean())
        print("###################################")
        print(algo.getAdaptiveStrategy())
        print(algo.getProjectionStrategy())
        # print "coefficients=", result.getCoefficients()
        residuals = result.getResiduals()
        print("residuals=", residuals)
        relativeErrors = result.getRelativeErrors()
        print("relativeErrors=", relativeErrors)
        isLeastSquaresPCE = result.isLeastSquares()
        assert isLeastSquaresPCE
        involvesModelSelectionPCE = result.involvesModelSelection()
        involvesModelSelection = adaptiveStrategy.involvesModelSelection()
        assert involvesModelSelection == involvesModelSelectionPCE

        # Post-process the results
        vector = ot.FunctionalChaosRandomVector(result)
        mean = vector.getMean()[0]
        print("mean=%.8f" % mean, "absolute error=%.10f" % abs(mean - im.expectation))
        variance = vector.getCovariance()[0, 0]
        print(
            "variance=%.8f" % variance,
            "absolute error=%.10f" % abs(variance - im.variance),
        )
        sensitivity = ot.FunctionalChaosSobolIndices(result)
        for i in range(dimension):
            value = sensitivity.getSobolIndex(i)
            print(
                "Sobol index",
                i,
                "= %.8f" % value,
                "absolute error=%.10f" % abs(value - sobolFirstOrderReference[i]),
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
                    "absolute error=%.10f" % abs(value - sobolSecondOrderReference[k]),
                )
                k = k + 1
        indices = ot.Indices([0, 1, 2])
        value = sensitivity.getSobolIndex(indices)
        print(
            "Sobol index",
            indices,
            "=%.8f" % value,
            "absolute error=%.10f" % abs(value - im.S123),
        )
        for i in range(dimension):
            value = sensitivity.getSobolTotalIndex(i)
            print(
                "Sobol total index",
                i,
                "=%.8f" % value,
                "absolute error=%.10f" % abs(value - sobolTotalReference[i]),
            )
        indices = ot.Indices(2)
        k = 0
        for i in range(dimension):
            indices[0] = i
            for j in range(i + 1, dimension):
                indices[1] = j
                value = sensitivity.getSobolTotalIndex(indices)
                print(
                    "Sobol total index",
                    indices,
                    "=%.8f" % value,
                    "absolute error=%.10f"
                    % abs(value - sobolInteractionTotalReference2[k]),
                )
                k = k + 1
        indices = ot.Indices([0, 1, 2])
        value = sensitivity.getSobolTotalIndex(indices)
        print(
            "Sobol total index ",
            indices,
            "=%.8f" % value,
            "absolute error=%.10f" % abs(value - sobolInteractionTotalReference3[0]),
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
                    "absolute error=%.10f" % abs(value - sobolGroupedIndex2[k]),
                )
                k = k + 1
        indices = ot.Indices([0, 1, 2])
        value = sensitivity.getSobolGroupedIndex(indices)
        print(
            "Sobol grouped index ",
            indices,
            "=%.8f" % value,
            "absolute error=%.10f" % abs(value - sobolGroupedIndex3[0]),
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
