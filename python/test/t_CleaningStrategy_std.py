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
maximumDimension = 100
maximumSize = 20
significanceFactor = 1e-4
adaptiveStrategy = ot.CleaningStrategy(
    productBasis, maximumDimension, maximumSize, significanceFactor
)

assert adaptiveStrategy.involvesModelSelection() is True

adaptiveStrategy.computeInitialBasis()
assert adaptiveStrategy.getCurrentVectorIndex() == 20

psi = adaptiveStrategy.getPsi()
assert len(psi) == 20

alpha_k = [3.5, 0.1, 0.0, -0.2, 0.0, 0.3, 0.0, -0.4, 0.0, -0.5]
residual = 0.0
relativeError = 0.0
adaptiveStrategy.updateBasis(alpha_k, residual, relativeError)

psi = adaptiveStrategy.getPsi()
assert len(psi) == 7
assert adaptiveStrategy.getCurrentVectorIndex() == 21
