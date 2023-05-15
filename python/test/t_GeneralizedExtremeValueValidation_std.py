#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp

ot.TESTPREAMBLE()

size = 10000
distribution = ot.GeneralizedExtremeValue(2.0, 1.5, -0.15)
sample = distribution.getSample(size)
# fake result
distributionParameter = ot.Normal(distribution.getParameter(), ot.CovarianceMatrix(3))
result = ot.DistributionFactoryResult(distribution, distributionParameter)
validation = otexp.GeneralizedExtremeValueValidation(result, sample)
graph1 = validation.drawReturnLevel()
assert len(graph1.getDrawables()) == 4, "nb drawables"
graph2 = validation.drawPDF()
assert len(graph2.getDrawables()) == 2, "nb drawables"
grid = validation.drawDiagnosticPlot()
assert grid.getNbColumns() == 2, "nb cols"
assert grid.getNbRows() == 2, "nb rows"
