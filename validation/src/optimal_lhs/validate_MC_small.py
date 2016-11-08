#! /usr/bin/env python

import openturns as ot
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from openturns.viewer import View
import time

ot.Log.Show(ot.Log.INFO)

# Bounds are [0,1]^dimension
dimension = 2
bounds = ot.Interval(dimension)

nSimu = 10000

c2 = ot.SpaceFillingC2()
# Size of sample
size = 10

print("dimension=%d, size=%d"%(dimension, size))
for nSimu in [100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600]:
    ot.RandomGenerator.SetSeed(0)
    # Factory: lhs generates
    lhsDesign = ot.LHSExperiment(ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dimension), size)
    lhsDesign.setAlwaysShuffle(True) # randomized

    mc = ot.MonteCarloLHS(lhsDesign, nSimu, c2)
    tic = time.time()
    design = mc.generate()
    result = mc.getResult()
    toc = time.time()
    print("%d %f %f"%(nSimu,result.getOptimalValue(), toc-tic))

pp = PdfPages('small_mc_OTLHS.pdf')
# plot criterion & save it
crit = result.drawHistoryCriterion()
fig = View(crit, plot_kwargs={'color':'blue'}).getFigure()
pp.savefig(fig)
plt.close(fig)
    
minDist = ot.SpaceFillingMinDist()

# Factory: lhs generates
lhsDesign = ot.LHSExperiment(ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dimension), size)
lhsDesign.setAlwaysShuffle(True) # randomized

mc = ot.MonteCarloLHS(lhsDesign, nSimu, minDist)
tic = time.time()
design = mc.generate()
result = mc.getResult()
toc = time.time()
print("cpu time=%f"%(toc-tic))
print("dimension=%d, size=%d,mc=%s"%(dimension, size, mc))
print("optimal value="+ str(result.getOptimalValue())+" c2="+str(result.getC2())+" phiP="+str(result.getPhiP())+" minDist="+str(result.getMinDist()))
# plot criterion & save it
crit = result.drawHistoryCriterion()
fig = View(crit, plot_kwargs={'color':'blue'}).getFigure()
pp.savefig(fig)
plt.close(fig)

pp.close()
