import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

dimension = 2
R = ot.CorrelationMatrix(dimension)
R[0, 1] = 0.8
distribution = ot.Normal([3.] * dimension, [2.] * dimension, R)
size = 100
sample = distribution.getSample(size)
firstSample = ot.NumericalSample(size, 1)
secondSample = ot.NumericalSample(size, 1)
for i in range(size):
    firstSample[i] = ot.NumericalPoint(1, sample[i, 0])
    secondSample[i] = ot.NumericalPoint(1, sample[i, 1])

lmtest = ot.LinearModelFactory().build(firstSample, secondSample)

drawLinearModel = ot.VisualTest_DrawLinearModel(
    firstSample, secondSample, lmtest)

View(drawLinearModel, figure_kwargs={'figsize': (5, 5)})
