#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from math import pi

ot.TESTPREAMBLE()

ot.PlatformInfo.SetNumericalPrecision(3)
# Problem parameters
dimension = 3
a = 7.0
b = 0.1
# Create the Ishigami function
inputVariables = ["xi1", "xi2", "xi3"]
formula = [
    "sin(xi1) + (" + str(a) + ") * (sin(xi2)) ^ 2 + (" + str(b) + ") * xi3^4 * sin(xi1)"
]
model = ot.SymbolicFunction(inputVariables, formula)

# Create the input distribution
distribution = ot.JointDistribution([ot.Uniform(-pi, pi)] * dimension)

# Fix sampling size
samplingSize = 100

# Get input & output sample
lhs = ot.LHSExperiment(distribution, samplingSize)
inputSample = lhs.generate()
outputSample = model(inputSample)

# Validation of results on independent samples
validationSize = 10
inputValidation = distribution.getSample(validationSize)
outputValidation = model(inputValidation)

# 1) SPC algorithm
# Create the orthogonal basis
polynomialCollection = [ot.LegendreFactory()] * dimension

enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(
    polynomialCollection, enumerateFunction
)

# Create the adaptive strategy
degree = 8
basisSize = enumerateFunction.getStrataCumulatedCardinal(degree)
adaptiveStrategy = ot.FixedStrategy(productBasis, basisSize)

# Select the fitting algorithm
fittingAlgorithm = ot.KFold()
leastSquaresFactory = ot.LeastSquaresMetaModelSelectionFactory(
    ot.LARS(), fittingAlgorithm
)

# Projection strategy
projectionStrategy = ot.LeastSquaresStrategy(
    inputSample, outputSample, leastSquaresFactory
)

algo = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, distribution, adaptiveStrategy, projectionStrategy
)
# Reinitialize the RandomGenerator to see the effect of the sampling
# method only
ot.RandomGenerator.SetSeed(0)
algo.run()

# Get the results
result = algo.getResult()

# MetaModelValidation - SPC
metamodel = result.getMetaModel()
metamodelPredictions = metamodel(inputValidation)
metaModelValidationSPC = ot.MetaModelValidation(outputValidation, metamodelPredictions)
print("")
print("Sparse chaos scoring")
print("R2 = ", metaModelValidationSPC.computeR2Score())
print("Residual sample = ", repr(metaModelValidationSPC.getResidualSample()))

# Weighted validation: scores against a brute-force recomputation
validationWeights = [0.5 + (i % 4) * 0.25 for i in range(validationSize)]
weightedValidation = ot.MetaModelValidation(outputValidation, metamodelPredictions)
weightedValidation.setWeights(validationWeights)
ott.assert_almost_equal(weightedValidation.getWeights(), validationWeights)
yValues = [outputValidation[i, 0] for i in range(validationSize)]
yHatValues = [metamodelPredictions[i, 0] for i in range(validationSize)]
weightSum = sum(validationWeights)
weightedMean = (
    sum(w * y for w, y in zip(validationWeights, yValues)) / weightSum
)
expectedVariance = (
    sum(w * (y - weightedMean) ** 2 for w, y in zip(validationWeights, yValues))
    / weightSum
)
expectedMSE = (
    sum(w * (y - yh) ** 2 for w, y, yh in zip(validationWeights, yValues, yHatValues))
    / weightSum
)
ott.assert_almost_equal(
    weightedValidation.computeMeanSquaredError()[0], expectedMSE, 1e-12, 1e-12
)
ott.assert_almost_equal(
    weightedValidation.computeR2Score()[0], 1.0 - expectedMSE / expectedVariance, 1e-12, 1e-12
)
# Uniform weights reproduce the unweighted scores
uniformValidation = ot.MetaModelValidation(outputValidation, metamodelPredictions)
uniformValidation.setWeights([2.0] * validationSize)
ott.assert_almost_equal(
    uniformValidation.computeR2Score(), metaModelValidationSPC.computeR2Score(), 1e-12, 1e-12
)
ott.assert_almost_equal(
    uniformValidation.computeMeanSquaredError(),
    metaModelValidationSPC.computeMeanSquaredError(),
    1e-12,
    1e-12,
)

# 2) GPR algorithm
basis = ot.QuadraticBasisFactory(dimension).build()
# model already computed, separately
covarianceModel = ot.GeneralizedExponential([3.52, 2.15, 2.99], [11.41], 2.0)
fitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
fitter.setOptimizeParameters(False)
fitter.run()
algo2 = ot.GaussianProcessRegression(fitter.getResult())
algo2.run()
result2 = algo2.getResult()

# MetaModelValidation - KG
metamodel = result2.getMetaModel()
metamodelPredictions = metamodel(inputValidation)
metaModelValidationKG = ot.MetaModelValidation(outputValidation, metamodelPredictions)
print("")
print("Kriging scoring")
print("R2 = ", metaModelValidationKG.computeR2Score())
ot.PlatformInfo.SetNumericalPrecision(2)
print("Residual sample = ", repr(metaModelValidationKG.getResidualSample()))

# 2-d
dist = ot.Uniform(-pi / 2, pi / 2)
model = ot.SymbolicFunction(["x"], ["sin(x)", "cos(x)"])
metaModel = ot.SymbolicFunction(["x"], ["x - x^3/6.0 + x^5/120.0", "cos(1.2*x)"])
x = dist.getSample(1000)
y = model(x)
metamodelPredictions = metaModel(x)
val = ot.MetaModelValidation(y, metamodelPredictions)
r2 = val.computeR2Score()
residual = val.getResidualSample()
residual_dist_smooth = val.getResidualDistribution()
residual_dist = val.getResidualDistribution(False)
graph = val.drawValidation()
print(r2)
