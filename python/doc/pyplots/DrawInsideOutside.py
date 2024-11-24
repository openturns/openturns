import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

domain = ot.Interval([-2.0, -1.0, 1.0], [2.0, 1.0, 3.0])
uniform = ot.Uniform(-3.0, 4.0)
dist = ot.JointDistribution([uniform] * 3)
sample = dist.getSample(200)
grid = ot.VisualTest.DrawInsideOutside(domain, sample)

View(grid, figure_kw={"figsize": (4.5, 4.5)})
