#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os
import re

ot.TESTPREAMBLE()

size = 10000
distribution = ot.GeneralizedExtremeValue(2.0, 1.5, -0.15)
sample = distribution.getSample(size)
# fake result
distributionParameter = ot.Normal(distribution.getParameter(), ot.CovarianceMatrix(3))
result = ot.DistributionFactoryResult(distribution, distributionParameter)
validation = ot.GeneralizedExtremeValueValidation(result, sample)
graph1 = validation.drawReturnLevel()
assert len(graph1.getDrawables()) == 4, "nb drawables"
graph2 = validation.drawPDF()
assert len(graph2.getDrawables()) == 2, "nb drawables"
grid = validation.drawDiagnosticPlot()
assert grid.getNbColumns() == 2, "nb cols"
assert grid.getNbRows() == 2, "nb rows"

# Archives predating confidenceLevel_ still load, keeping the default
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("gevLegacy.xml"))
study.add("validation", validation)
study.save()
with open("gevLegacy.xml") as f:
    content = f.read()
with open("gevLegacy.xml", "w") as f:
    f.write(re.sub(r' confidenceLevel_="[^"]*"', "", content))
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("gevLegacy.xml"))
study.load()
legacy = ot.GeneralizedExtremeValueValidation()
study.fillObject("validation", legacy)
ott.assert_almost_equal(legacy.getConfidenceLevel(), 0.95, 0.0, 0.0)
os.remove("gevLegacy.xml")
