#! /usr/bin/env python

import openturns as ot
from openturns.usecases import flood_model

ot.TESTPREAMBLE()

ot.Log.Show(ot.Log.NONE)

sampleSize = 500
totalDegree = 7

fm = flood_model.FloodModel()
inputDescription = fm.model.getInputDescription()
marginals = [fm.distribution.getMarginal(i) for i in range(fm.dim)]
basis = ot.OrthogonalProductPolynomialFactory(marginals)
inputSample = fm.distribution.getSample(sampleSize)
outputSample = fm.model(inputSample)
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(selectionAlgorithm)
enumerateFunction = basis.getEnumerateFunction()
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
adaptiveStrategy = ot.FixedStrategy(basis, basisSize)
algo = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, fm.distribution, adaptiveStrategy, projectionStrategy
)
algo.run()
chaosResult = algo.getResult()
print(chaosResult)
print(chaosResult._repr_html_())
print(chaosResult.__repr_markdown__())


class RepeatedFloodOutputDimensionFunction(ot.OpenTURNSPythonFunction):
    def __init__(self, outputDimension):
        super().__init__(4, outputDimension)
        self.fm = flood_model.FloodModel()
        self.outputDimension = outputDimension

    def _exec(self, x):
        y = ot.Point(self.outputDimension)
        y_base = fm.model(x)
        for i in range(self.outputDimension):
            y[i] = i + y_base[0]
        return y


# Test pretty-print with large dimension output
outputDimension = 20
model = ot.Function(RepeatedFloodOutputDimensionFunction(outputDimension))
sampleSize = 500
totalDegree = 7
fm = flood_model.FloodModel()
inputDescription = fm.model.getInputDescription()
marginals = [fm.distribution.getMarginal(i) for i in range(fm.dim)]
basis = ot.OrthogonalProductPolynomialFactory(marginals)
inputSample = fm.distribution.getSample(sampleSize)
outputSample = model(inputSample)
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(selectionAlgorithm)
enumerateFunction = basis.getEnumerateFunction()
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
adaptiveStrategy = ot.FixedStrategy(basis, basisSize)
chaosAlgo = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, fm.distribution, adaptiveStrategy, projectionStrategy
)
chaosAlgo.run()
chaosResult = chaosAlgo.getResult()
print(chaosResult)
print(chaosResult._repr_html_())
print(chaosResult._repr_markdown_())

composedMetamodel = chaosResult.getComposedMetaModel()
print("Composed metamodel =")
print(composedMetamodel._repr_html_())

metamodel = chaosResult.getMetaModel()
print("Metamodel =")
print(metamodel._repr_html_())
