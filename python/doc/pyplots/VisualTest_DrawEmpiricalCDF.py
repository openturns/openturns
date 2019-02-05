import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)

sampleCDF = ot.VisualTest.DrawEmpiricalCDF(
    sample, sample.getMin()[0] - 1.0, sample.getMax()[0] + 1.0)

View(sampleCDF, figure_kwargs={'figsize': (4.5, 4.5)})
