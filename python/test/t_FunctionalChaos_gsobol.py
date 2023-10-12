#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


def sobol(indice, ai):
    val = 1.0
    for i in range(indice.getSize()):
        val = val * 1.0 / (3.0 * (1.0 + ai[indice[i]]) ** 2.0)
    return val


# Problem parameters
dimension = 3

# Create the Sobol function
# Reference analytical values
meanTh = 1.0
a = ot.Point(dimension)
inputVariables = ot.Description(dimension)
formula = ot.Description(1)
formula[0] = "1.0"
covTh = 1.0
for i in range(dimension):
    a[i] = 0.5 * i
    covTh = covTh * (1.0 + 1.0 / (3.0 * (1.0 + a[i]) ** 2))
    inputVariables[i] = "xi" + str(i)
    formula[0] = (
        formula[0]
        + " * ((abs(4.0 * xi"
        + str(i)
        + " - 2.0) + "
        + str(a[i])
        + ") / (1.0 + "
        + str(a[i])
        + "))"
    )
covTh = covTh - 1.0
model = ot.SymbolicFunction(inputVariables, formula)

# Create the input distribution
marginals = [ot.Uniform(0.0, 1.0)] * dimension
distribution = ot.ComposedDistribution(marginals)

# Create the orthogonal basis
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(marginals)

# Create the adaptive strategy
# We can choose amongst several strategies
# First, the most efficient (but more complex!) strategy
listAdaptiveStrategy = list()
degree = 4
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
    # Create the projection strategy
    samplingSize = 250
    listExperiment = list()
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
        print("###################################")
        print(algo.getAdaptiveStrategy())
        print(algo.getProjectionStrategy())
        # print "coefficients=", result.getCoefficients()
        residuals = result.getResiduals()
        print("residuals=", residuals)
        relativeErrors = result.getRelativeErrors()
        print("relative errors=", relativeErrors)

        # Post-process the results
        vector = ot.FunctionalChaosRandomVector(result)
        mean = vector.getMean()[0]
        print("mean=%.8f" % mean, "absolute error=%.8f" % abs(mean - meanTh))
        variance = vector.getCovariance()[0, 0]
        sensitivity = ot.FunctionalChaosSobolIndices(result)
        print("variance=%.8f" % variance, "absolute error=%.8f" % abs(variance - covTh))
        indices = ot.Indices(1)
        for i in range(dimension):
            indices[0] = i
            value = sensitivity.getSobolIndex(i)
            print(
                "Sobol index",
                i,
                "= %.8f" % value,
                "absolute error=%.8f" % abs(value - sobol(indices, a) / covTh),
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
                    "absolute error=%.8f" % abs(value - sobol(indices, a) / covTh),
                )
                k = k + 1
        indices = ot.Indices([0, 1, 2])
        value = sensitivity.getSobolIndex(indices)
        print(
            "Sobol index",
            indices,
            "=%.8f" % value,
            "absolute error=%.8f" % abs(value - sobol(indices, a) / covTh),
        )
