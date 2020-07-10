import openturns as ot
from openturns.viewer import View
import math as m

ot.RandomGenerator.SetSeed(0)

# 3-d test
R1 = ot.CovarianceMatrix(3)
R1[2,1] = -0.25
R2 = ot.CovarianceMatrix(3)
R2[1,0] = 0.5
R2[2,1] = -0.3
R2[0,0] = 1.3
print(R2)
dists = [ot.Normal([1.0, -2.0, 3.0], R1), ot.Normal([-1.0, 2.0, -2.0], R2)]
mixture = ot.Mixture(dists, [2.0 / 3.0, 1.0 / 3.0])

# 2-d test
dists = [ot.Normal([-1.0, 2.0], [1.0]*2, ot.CorrelationMatrix(2)), ot.Normal([1.0, -2.0], [1.5]*2, ot.CorrelationMatrix(2))]
mixture = ot.Mixture(dists)

sample = mixture.getSample(100)
distribution = ot.KernelSmoothing().build(sample)
algo = ot.MinimumVolumeClassifier(distribution, 0.8)
graph = algo.drawContourAndSample([0.1, 0.5, 0.8], sample, [0, 1])

View(graph, contour_kw={'colors':['black']}, figure_kw={'figsize': (6.0, 6.0)})
