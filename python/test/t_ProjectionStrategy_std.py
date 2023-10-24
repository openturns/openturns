#! /usr/bin/env python

import openturns as ot
from openturns.usecases import flood_model

ot.TESTPREAMBLE()

ot.Log.Show(ot.Log.NONE)

sampleSize = 500
totalDegree = 7

#
print("+ Compute flood model using least squares")
fm = flood_model.FloodModel()
inputDescription = fm.model.getInputDescription()
marginals = [fm.distribution.getMarginal(i) for i in range(fm.dim)]
basis = ot.OrthogonalProductPolynomialFactory(marginals)
inputSample = fm.distribution.getSample(sampleSize)
outputSample = fm.model(inputSample)
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
leastSquaresStrategy = ot.LeastSquaresStrategy(selectionAlgorithm)
print("leastSquaresStrategy")
print(leastSquaresStrategy)
print("leastSquaresStrategy (repr)")
print(leastSquaresStrategy.__repr__())
print("leastSquaresStrategy (html)")
print(leastSquaresStrategy._repr_html_())

enumerateFunction = basis.getEnumerateFunction()
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
adaptiveStrategy = ot.FixedStrategy(basis, basisSize)
chaosAlgo = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, fm.distribution, adaptiveStrategy, leastSquaresStrategy
)
projectionStrategy = chaosAlgo.getProjectionStrategy()
print("projectionStrategy")
print(projectionStrategy)
print("projectionStrategy (repr)")
print(projectionStrategy.__repr__())
print("projectionStrategy (html)")
print(projectionStrategy._repr_html_())


#
print("+ Compute flood model by integration")
integrationStrategy = ot.IntegrationStrategy()
print("integrationStrategy")
print(integrationStrategy)
print("integrationStrategy (repr)")
print(integrationStrategy.__repr__())
print("integrationStrategy (html)")
print(integrationStrategy._repr_html_())
chaosAlgo = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, fm.distribution, adaptiveStrategy, integrationStrategy
)
projectionStrategy = chaosAlgo.getProjectionStrategy()
print("projectionStrategy")
print(projectionStrategy)
print("projectionStrategy (repr)")
print(projectionStrategy.__repr__())
print("projectionStrategy (html)")
print(projectionStrategy._repr_html_())

#
print("+ Compute flood model with large output dimension")
outputDimension = 20
fm = flood_model.FloodModel()
model = ot.AggregatedFunction([fm.model] * outputDimension)
sampleSize = 500
totalDegree = 7
fm = flood_model.FloodModel()
inputDescription = fm.model.getInputDescription()
marginals = [fm.distribution.getMarginal(i) for i in range(fm.dim)]
basis = ot.OrthogonalProductPolynomialFactory(marginals)
inputSample = fm.distribution.getSample(sampleSize)
outputSample = model(inputSample)
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
leastSquaresStrategy = ot.LeastSquaresStrategy(selectionAlgorithm)
print("leastSquaresStrategy")
print(leastSquaresStrategy)
print("leastSquaresStrategy (repr)")
print(leastSquaresStrategy.__repr__())
print(leastSquaresStrategy._repr_html_())

enumerateFunction = basis.getEnumerateFunction()
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
adaptiveStrategy = ot.FixedStrategy(basis, basisSize)
chaosAlgo = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, fm.distribution, adaptiveStrategy, leastSquaresStrategy
)
projectionStrategy = chaosAlgo.getProjectionStrategy()
print("projectionStrategy")
print(projectionStrategy)
print("projectionStrategy (repr)")
print(projectionStrategy.__repr__())
print("projectionStrategy (html)")
print(projectionStrategy._repr_html_())
