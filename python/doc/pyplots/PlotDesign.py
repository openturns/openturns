import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import PlotDesign

ot.RandomGenerator.SetSeed(0)

dim = 3
size = 10
distribution = ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dim)
bounds = distribution.getRange()
lhs = ot.LHSExperiment(distribution, size)
design = lhs.generate()
subdivisions = [size] * dim

fig = plt.figure(figsize=(8, 8))
PlotDesign(design, bounds, subdivisions, figure=fig)
