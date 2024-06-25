#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

dimension = 3

# Create the orthogonal basis
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * dimension, enumerateFunction
)

degree = 6
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(degree)
adaptiveStrategy = ot.FixedStrategy(productBasis, basisSize)

assert adaptiveStrategy.involvesModelSelection() is False

adaptiveStrategy.computeInitialBasis()

alpha_k = [3.5, 0.1, 0.0, -0.2, 0.0, 0.3, 0.0, -0.4, 0.0, -0.5]
residual = 0.0
relativeError = 0.0
adaptiveStrategy.updateBasis(alpha_k, residual, relativeError)

psi = adaptiveStrategy.getPsi()
assert len(psi) == basisSize
