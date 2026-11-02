import openturns as ot
import openturns.viewer as otv


ot.RandomGenerator.SetSeed(0)
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 3)
sample = distribution.getSample(10)
grid = ot.VisualTest.DrawPairsWithGrid(sample, distribution.getRange(), [10, 10, 10])

otv.View(grid, figure_kw={"figsize": (6.0, 6.0)})
