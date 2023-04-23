#! /usr/bin/env python

import openturns as ot
from math import pi

ot.TESTPREAMBLE()

#! /usr/bin/env python

import openturns as ot
import math as m
from openturns.usecases import flood_model

sampleSize = 500
totalDegree = 7

#
fm = flood_model.FloodModel()
inputDescription = fm.model.getInputDescription()

#
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
result = algo.getResult()
print(result)
