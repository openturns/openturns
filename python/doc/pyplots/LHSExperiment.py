import openturns as ot
from matplotlib import pyplot as plt
import openturns.viewer as otv
ot.RandomGenerator.SetSeed(0)

distribution = ot.ComposedDistribution([ot.Uniform()]*3)
samplesize = 5
experiment = ot.LHSExperiment(distribution, samplesize, False, False)
sample = experiment.generate()

bounds = distribution.getRange()

otv.PlotDesign(sample, bounds)

