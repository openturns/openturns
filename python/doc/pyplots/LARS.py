#!/usr/bin/env python

import openturns as ot
from openturns.usecases import ishigami_function
from openturns.viewer import View

# data
ot.RandomGenerator.SetSeed(0)
im = ishigami_function.IshigamiModel()
N = 1000
g = im.model
x = im.distributionX.getSample(N)
P = x.getDimension()
marginals = [im.distributionX.getMarginal(i) for i in range(P)]
y = g(x)

# polynomial chaos
q, totalDegree = 0.4, 5
enumerateFunction = ot.HyperbolicAnisotropicEnumerateFunction(P, q)
productBasis = ot.OrthogonalProductPolynomialFactory(marginals, enumerateFunction)
approximationAlgorithm = ot.LeastSquaresMetaModelSelectionFactory(
    ot.LARS(), ot.CorrectedLeaveOneOut()
)
adaptiveStrategy = ot.FixedStrategy(
    productBasis, enumerateFunction.getStrataCumulatedCardinal(totalDegree)
)
projectionStrategy = ot.LeastSquaresStrategy(approximationAlgorithm)
algo = ot.FunctionalChaosAlgorithm(
    x, y, im.distributionX, adaptiveStrategy, projectionStrategy
)
algo.run()
result = algo.getResult()
graph = result.drawSelectionHistory()
View(graph)
