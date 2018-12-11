import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

dimension = 2
R = ot.CorrelationMatrix(dimension)
R[0, 1] = 0.8
size = 100
sample1 = ot.Normal([3.] * dimension, [2.] * dimension, R).getSample(size)
sample2 = ot.Normal([2.] * dimension, [3.] * dimension, R).getSample(size // 2)

samplesClouds = ot.VisualTest.DrawClouds(sample1, sample2)

View(samplesClouds, figure_kwargs={'figsize': (4, 4)})
