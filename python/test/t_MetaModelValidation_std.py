#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from math import pi

ot.TESTPREAMBLE()

try:

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
    print(
        "Q2 = ", round(metaModelValidationSPC.computePredictivityFactor(), 5))
    print("Residual sample = ", repr(
        metaModelValidationSPC.getResidualSample()))

    # 2) Kriging algorithm
    # KrigingAlgorithm
    basis = ot.QuadraticBasisFactory(dimension).build()
    # model already computed, separatly
    covarianceModel = ot.GeneralizedExponential(
        [1.933, 1.18, 1.644], [10.85], 2.0)
    algo2 = ot.KrigingAlgorithm(
        inputSample, outputSample, covarianceModel, basis, True)
    algo2.setOptimizeParameters(False)
    algo2.run()
    result2 = algo2.getResult()

    # MetaModelValidation - KG
    metaModelValidationKG = ot.MetaModelValidation(
        inputValidation, outputValidation, result2.getMetaModel())
    print("")
    print("Kriging scoring")
    print("Q2 = ", round(metaModelValidationKG.computePredictivityFactor(), 3))
    ot.PlatformInfo.SetNumericalPrecision(2)
    print("Residual sample = ", repr(
        metaModelValidationKG.getResidualSample()))

except:
    import sys
    print("t_MetaModelValidation_std.py", sys.exc_info()[0], sys.exc_info()[1])
