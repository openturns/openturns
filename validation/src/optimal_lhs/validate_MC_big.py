#! /usr/bin/env python

import openturns as ot
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from openturns.viewer import View
import time

ot.RandomGenerator.SetSeed(0)
ot.Log.Show(ot.Log.INFO)

# Bounds are [0,1]^dimension
dimension = 50

nSimu = 10000
c2 = ot.SpaceFillingC2()
pp = PdfPages('large_mc_OTLHS.pdf')

# Size of sample
size = 100
# Factory: lhs generates
lhsDesign = ot.LHSExperiment(ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dimension), size)
lhsDesign.setAlwaysShuffle(True) # randomized

mc = ot.MonteCarloLHS(lhsDesign, nSimu, c2)
tic = time.time()
design = mc.generate()
result = mc.getResult()
toc = time.time()
print("cpu time=%f"%(toc-tic))
print("dimension=%d, size=%d,mc=%s"%(dimension, size, mc))
print("optimal value="+ str(result.getOptimalValue())+" c2="+str(result.getC2())+" phiP="+str(result.getPhiP())+" minDist="+str(result.getMinDist()))
# plot criterion
crit = result.drawHistoryCriterion()
# in pdf
pp.savefig(View(crit, plot_kwargs={'color':'blue'}).getFigure())

minDist = ot.SpaceFillingMinDist()
# Factory: lhs generates
lhsDesign = ot.LHSExperiment(ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dimension), size)
lhsDesign.setAlwaysShuffle(True) # randomized

mc = ot.MonteCarloLHS(lhsDesign, nSimu, minDist)
tic = time.time()
result = mc.generate()
toc = time.time()
print("cpu time=%f"%(toc-tic))
print("dimension=%d, size=%d,mc=%s"%(dimension, size, mc))
print("optimal value="+ str(result.getOptimalValue())+" c2="+str(result.getC2())+" phiP="+str(result.getPhiP())+" minDist="+str(result.getMinDist()))
# plot criterion
crit = result.drawHistoryCriterion()
pp.savefig(View(crit, plot_kwargs={'color':'blue'}).getFigure())

pp.close()
