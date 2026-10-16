import openturns as ot
import openturns.viewer as otv


ot.RandomGenerator.SetSeed(0)
bounds = ot.Interval([0.0, 0.0], [1.0, 1.0])
grid = ot.Grid(bounds, [5, 5])
cloud = ot.Cloud(ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 2).getSample(10))
graph = ot.Graph()
graph.add(grid)
graph.add(cloud)

otv.View(graph)
