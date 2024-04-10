#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()


f1 = ot.SymbolicFunction(
    ["a0", "a1"],
    ["-4 * exp((-25 / 8) * (a0^2 + a1^2)) + 7 * exp((-125 / 4) * (a0^2 + a1^2))"],
)

distribution = ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * 2)
x0 = ot.LowDiscrepancyExperiment(ot.HaltonSequence(), distribution, 100).generate()
y0 = f1(x0)

ref_mean = [
    [-0.120703, -0.0386556],
    [0.0751319, -0.00342476],
    [0.0405645, 0.00620636],
    [0.00230313, -0.00323881],
    [0.129622, -0.0752325],
]

algo = otexp.LOLAVoronoi(x0, y0, distribution)
newX = ot.Sample(0, x0.getDimension())
for i in range(5):
    x0 = algo.getX()
    x = algo.generate(15)
    newX.add(x)
    y = f1(x)
    algo.update(x, y)
    x_mean = x.computeMean()
    print(f"iteration={i} x_mean={x_mean}")

    # FIXME: legacy KDTree is incorrect https://github.com/openturns/openturns/issues/2617
    if ot.PlatformInfo.HasFeature("nanoflann"):
        ott.assert_almost_equal(x_mean, ref_mean[i])

    if False:
        import openturns.viewer as otv

        cloud1 = ot.Cloud(x0)
        cloud1.setPointStyle("fcircle")
        cloud1.setColor("blue")
        cloud2 = ot.Cloud(newX)
        cloud2.setPointStyle("fcircle")
        cloud2.setColor("red")
        graph = ot.Graph("LOLA-Voronoi", "x1", "x2", True)
        graph.add(cloud1)
        graph.add(cloud2)
        otv.View(graph)
        otv.View.ShowAll()
