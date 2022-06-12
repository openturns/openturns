#! /usr/bin/env python

import openturns as ot
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
    "sin(xi1) + (" + str(a) + ") * (sin(xi2)) ^ 2 + (" + str(b) + ") * xi3^4 * sin(xi1)"]
model = ot.SymbolicFunction(inputVariables, formula)

# Create the input distribution
distribution = ot.ComposedDistribution([ot.Uniform(-pi, pi)] * dimension)

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
    polynomialCollection, enumerateFunction)

# Create the adaptive strategy
degree = 8
basisSize = enumerateFunction.getStrataCumulatedCardinal(degree)
adaptiveStrategy = ot.FixedStrategy(productBasis, basisSize)

# Select the fitting algorithm
fittingAlgorithm = ot.KFold()
leastSquaresFactory = ot.LeastSquaresMetaModelSelectionFactory(
    ot.LARS(), fittingAlgorithm)

# Projection strategy
projectionStrategy = ot.LeastSquaresStrategy(
    inputSample, outputSample, leastSquaresFactory)

algo = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, distribution, adaptiveStrategy, projectionStrategy)
# Reinitialize the RandomGenerator to see the effect of the sampling
# method only
ot.RandomGenerator.SetSeed(0)
algo.run()

# Get the results
result = algo.getResult()

# MetaModelValidation - SPC
metaModelValidationSPC = ot.MetaModelValidation(
    inputValidation, outputValidation, result.getMetaModel())
print("")
print("Sparse chaos scoring")
print("Q2 = ", metaModelValidationSPC.computePredictivityFactor())
print("Residual sample = ", repr(
    metaModelValidationSPC.getResidualSample()))

# 2) Kriging algorithm
# KrigingAlgorithm
basis = ot.QuadraticBasisFactory(dimension).build()
# model already computed, separately
covarianceModel = ot.GeneralizedExponential(
    [3.52, 2.15, 2.99], [11.41], 2.0)
algo2 = ot.KrigingAlgorithm(
    inputSample, outputSample, covarianceModel, basis)
algo2.setOptimizeParameters(False)
algo2.run()
result2 = algo2.getResult()

# MetaModelValidation - KG
metaModelValidationKG = ot.MetaModelValidation(
    inputValidation, outputValidation, result2.getMetaModel())
print("")
print("Kriging scoring")
print("Q2 = ", metaModelValidationKG.computePredictivityFactor())
ot.PlatformInfo.SetNumericalPrecision(2)
print("Residual sample = ", repr(
    metaModelValidationKG.getResidualSample()))

# 2-d
dist = ot.Uniform(-pi/2, pi/2)
model = ot.SymbolicFunction(['x'], ['sin(x)', 'cos(x)'])
metaModel = ot.SymbolicFunction(
    ['x'], ['x - x^3/6.0 + x^5/120.0', 'cos(1.2*x)'])
x = dist.getSample(1000)
y = model(x)
val = ot.MetaModelValidation(x, y, metaModel)
q2 = val.computePredictivityFactor()
residual = val.getResidualSample()
residual_dist_smooth = val.getResidualDistribution()
residual_dist = val.getResidualDistribution(False)
graph = val.drawValidation()
print(q2)
