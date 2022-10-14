#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# distributions
continuous_1D_dist = ot.Normal()
d = 3
finite_1D_dist = ot.Binomial(d - 1, 0.5)
marginals = [continuous_1D_dist, finite_1D_dist]
mixed_2D_dist = ot.ComposedDistribution(marginals)

# data
N = 1000
g = ot.SymbolicFunction(["X1", "X2"], ["sin(X1) + X2"])
x = mixed_2D_dist.getSample(N)
y = g(x)

# polynomial chaos
q, totalDegree = 0.4, 5
P = x.getDimension()
enumerateFunction = ot.HyperbolicAnisotropicEnumerateFunction(P, q)
productBasis = ot.OrthogonalProductPolynomialFactory(marginals, enumerateFunction)
fittingAlgorithm = ot.CorrectedLeaveOneOut()
approximationAlgorithm = ot.LeastSquaresMetaModelSelectionFactory(
    ot.LARS(), fittingAlgorithm
)
adaptiveStrategy = ot.FixedStrategy(
    productBasis, enumerateFunction.getStrataCumulatedCardinal(totalDegree)
)
projectionStrategy = ot.LeastSquaresStrategy(approximationAlgorithm)
algo = ot.FunctionalChaosAlgorithm(
    x, y, mixed_2D_dist, adaptiveStrategy, projectionStrategy
)
algo.run()
result = algo.getResult()
print(result.getRelativeErrors())
assert result.getRelativeErrors()[0] < 1e-10
