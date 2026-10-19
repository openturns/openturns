#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os
import re

ot.TESTPREAMBLE()

# 2000 points are enough: only the drawable counts are asserted below,
# and the diagnostic plots sort the sample (quadratic cost)
size = 2000
distribution = ot.GeneralizedPareto(1.5, -0.2, 0.5)
sample = distribution.getSample(size)
# fake result
distributionParameter = ot.Normal(distribution.getParameter(), ot.CovarianceMatrix(3))
result = ot.DistributionFactoryResult(distribution, distributionParameter)
validation = ot.GeneralizedParetoValidation(result, sample)
graph1 = validation.drawReturnLevel()
assert len(graph1.getDrawables()) == 4, "nb drawables"
graph2 = validation.drawPDF()
assert len(graph2.getDrawables()) == 2, "nb drawables"
grid = validation.drawDiagnosticPlot()
assert grid.getNbColumns() == 2, "nb cols"
assert grid.getNbRows() == 2, "nb rows"

# Archives predating confidenceLevel_ still load, keeping the default
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("gpvLegacy.xml"))
study.add("validation", validation)
study.save()
with open("gpvLegacy.xml") as f:
    content = f.read()
with open("gpvLegacy.xml", "w") as f:
    f.write(re.sub(r' confidenceLevel_="[^"]*"', "", content))
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("gpvLegacy.xml"))
study.load()
legacy = ot.GeneralizedParetoValidation()
study.fillObject("validation", legacy)
ott.assert_almost_equal(legacy.getConfidenceLevel(), 0.95, 0.0, 0.0)
os.remove("gpvLegacy.xml")
