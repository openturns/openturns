#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import sys
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()

ot.ResourceMap.SetAsBool('Distribution-MinimumVolumeLevelSetBySampling', True)
ot.ResourceMap.SetAsUnsignedInteger(
    'Distribution-MinimumVolumeLevelSetSamplingSize', 500)

# 2-d test
dists = [ot.Normal([-1.0, 2.0], [1.0]*2, ot.CorrelationMatrix(2)),
         ot.Normal([1.0, -2.0], [1.5]*2, ot.CorrelationMatrix(2))]
mixture = ot.Mixture(dists)

# 3-d test
R1 = ot.CovarianceMatrix(3)
R1[2, 1] = -0.25
R2 = ot.CovarianceMatrix(3)
R2[1, 0] = 0.5
R2[2, 1] = -0.3
R2[0, 0] = 1.3
print(R2)
dists = [ot.Normal([1.0, -2.0, 3.0], R1), ot.Normal([-1.0, 2.0, -2.0], R2)]
mixture = ot.Mixture(dists, [2.0 / 3.0, 1.0 / 3.0])


sample = mixture.getSample(1000)
distribution = ot.KernelSmoothing().build(sample)
algo = ot.MinimumVolumeClassifier(distribution, 0.8)
threshold = algo.getThreshold()
print("threshold=", threshold)
assert m.fabs(threshold - 0.0012555) < 1e-3, "wrong threshold"
cls_ref = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1]
for i in range(35):
    x = sample[i]
    cls = algo.classify(x)
    pdf = mixture.computePDF(x)
    print(i, x, cls, pdf - threshold)
    assert cls == cls_ref[i], "wrong class"

graph1 = algo.drawSample(sample, [0])
graph2 = algo.drawSample(sample, [1])
graph3 = algo.drawSample(sample, [0, 1])
contour_alpha = [0.1, 0.5, 0.8]
graph4 = algo.drawContour(contour_alpha)
graph5 = algo.drawContourAndSample(contour_alpha, sample, [0, 1])

if len(sys.argv) > 1:
    from openturns.viewer import View
    # View(graph3).show()
    # View(graph4).show()
    view = View(graph5)
    view.save('mvc.png')
    view.ShowAll()
