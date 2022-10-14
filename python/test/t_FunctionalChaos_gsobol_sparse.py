#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Log.Show( Log.Flags() | Log.INFO )

# Problem parameters
dimension = 8

# Create the Ishigami function
# Reference analytical values
meanTh = 1.0
covTh = 1.0
a = ot.Point(dimension)
a[0] = 1.0
a[1] = 2.0
a[2] = 5.0
a[3] = 10.0
a[4] = 20.0
a[5] = 50.0
a[6] = 100.0
a[7] = 500.0
inputVariables = ot.Description(dimension)
formula = ot.Description(1)
formula[0] = "1.0"
for i in range(dimension):
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
distribution = ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dimension)

# Create the orthogonal basis
q = 0.4
enumerateFunction = ot.HyperbolicAnisotropicEnumerateFunction(dimension, q)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * dimension, enumerateFunction
)

# design experiment
samplingSize = 75

# build basis
basisSize = enumerateFunction.getStrataCumulatedCardinal(30)

listFittingAlgorithm = list()
listFittingAlgorithm.append(ot.KFold())
listFittingAlgorithm.append(ot.CorrectedLeaveOneOut())

for fittingAlgorithmIndex in range(len(listFittingAlgorithm)):
    fittingAlgorithm = listFittingAlgorithm[fittingAlgorithmIndex]
    adaptiveStrategy = ot.FixedStrategy(productBasis, basisSize)
    projectionStrategy = ot.LeastSquaresStrategy(
        ot.LeastSquaresMetaModelSelectionFactory(ot.LARS(), fittingAlgorithm)
    )
    experiment = ot.LowDiscrepancyExperiment(
        ot.SobolSequence(), distribution, samplingSize
    )
    ot.RandomGenerator.SetSeed(0)
    X = experiment.generate()
    Y = model(X)
    algo = ot.FunctionalChaosAlgorithm(
        X, Y, distribution, adaptiveStrategy, projectionStrategy
    )
    algo.run()

    result = algo.getResult()
    print("coeffs = ", result.getCoefficients())
    print("residuals = ", result.getResiduals())
    print("relative errors = ", result.getRelativeErrors())
