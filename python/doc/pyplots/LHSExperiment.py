import openturns as ot
import openturns.viewer as otv

dim = 3
size = 10
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * dim)
bounds = distribution.getRange()
lhs = ot.LHSExperiment(distribution, size)
sample = lhs.generate()
subdivisions = [size] * dim
fig = otv.PlotDesign(sample, bounds, subdivisions)
