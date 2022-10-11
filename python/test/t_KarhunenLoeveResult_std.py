import openturns as ot
import unittest
import openturns.testing as ott
import sys

ot.TESTPREAMBLE()

# Create the KL result
numberOfVertices = 20
interval = ot.Interval(-1.0, 1.0)
mesh = ot.IntervalMesher([numberOfVertices - 1]).build(interval)
covariance = ot.SquaredExponential()
process = ot.GaussianProcess(covariance, mesh)
sampleSize = 100
processSample = process.getSample(sampleSize)
threshold = 1.0e-7
algo = ot.KarhunenLoeveSVDAlgorithm(processSample, threshold)
algo.run()
klresult = algo.getResult()
print("ev=", klresult.getEigenvalues())
graph1 = klresult.drawEigenvalues()
graph2 = klresult.drawEigenvalues()
graph2.setLogScale(ot.GraphImplementation.LOGY)
graph3 = klresult.drawCumulatedEigenvaluesRemainder()
graph4 = klresult.drawCumulatedEigenvaluesRemainder()
graph4.setLogScale(ot.GraphImplementation.LOGY)
if 0:
    from openturns.viewer import View

    View(graph1).save("eigen1.png")
    View(graph2).save("eigen2.png")
    View(graph3).save("eigen3.png")
    View(graph4).save("eigen4.png")
