#! /usr/bin/env python

import openturns as ot
import math
from openturns.usecases import ishigami_function

ot.TESTPREAMBLE()

# Create Ishigami
im = ishigami_function.IshigamiModel()
input_names = im.distributionX.getDescription()
sampleSize = 500
inputSample = im.distributionX.getSample(sampleSize)
outputSample = im.model(inputSample)

# Create PCE
multivariateBasis = ot.OrthogonalProductPolynomialFactory([im.X1, im.X2, im.X3])
totalDegree = 10
enumerateFunction = multivariateBasis.getEnumerateFunction()
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(
    inputSample, outputSample, selectionAlgorithm
)
adaptiveStrategy = ot.FixedStrategy(multivariateBasis, basisSize)
chaosAlgorithm = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, im.distributionX, adaptiveStrategy, projectionStrategy
)
chaosAlgorithm.run()
chaosResult = chaosAlgorithm.getResult()

# PCE | X1
conditionalPCE = chaosResult.getConditionalExpectation([0])
conditionalPCEFunction = conditionalPCE.getMetaModel()
parametricEgivenX1 = ot.SymbolicFunction(
    ["xi1", "a", "b"], ["a / 2 + (1 + b * pi_^4 / 5) * sin(xi1)"]
)
functionEgivenX1Exact = ot.ParametricFunction(parametricEgivenX1, [1, 2], [im.a, im.b])
sampleSizeTest = 10000
absoluteTolerance = 1.0 / math.sqrt(sampleSizeTest)
print("absoluteTolerance = ", absoluteTolerance)
marginalDistribution = im.distributionX.getMarginal(0)
experiment = ot.MonteCarloExperiment(marginalDistribution, sampleSizeTest)
integration = ot.ExperimentIntegration(experiment)
error = integration.computeL2Norm(conditionalPCEFunction - functionEgivenX1Exact)
print("PCE | X1, error = ", error)
assert error[0] < absoluteTolerance

# PCE | X2
conditionalPCE = chaosResult.getConditionalExpectation([1])
conditionalPCEFunction = conditionalPCE.getMetaModel()
parametricEgivenX2 = ot.SymbolicFunction(["xi2", "a", "b"], ["a * sin(xi2)^2"])
functionEgivenX2Exact = ot.ParametricFunction(parametricEgivenX2, [1, 2], [im.a, im.b])
marginalDistribution = im.distributionX.getMarginal(1)
experiment = ot.MonteCarloExperiment(marginalDistribution, sampleSizeTest)
integration = ot.ExperimentIntegration(experiment)
error = integration.computeL2Norm(conditionalPCEFunction - functionEgivenX2Exact)
print("PCE | X2, error = ", error)
assert error[0] < absoluteTolerance

# PCE | X3
conditionalPCE = chaosResult.getConditionalExpectation([2])
conditionalPCEFunction = conditionalPCE.getMetaModel()
parametricEgivenX3 = ot.SymbolicFunction(["xi1", "a", "b"], ["a / 2"])
functionEgivenX3Exact = ot.ParametricFunction(parametricEgivenX3, [1, 2], [im.a, im.b])
sampleSizeTest = 10000
marginalDistribution = im.distributionX.getMarginal(2)
experiment = ot.MonteCarloExperiment(marginalDistribution, sampleSizeTest)
integration = ot.ExperimentIntegration(experiment)
error = integration.computeL2Norm(conditionalPCEFunction - functionEgivenX3Exact)
print("PCE | X3, error = ", error)
assert error[0] < absoluteTolerance
