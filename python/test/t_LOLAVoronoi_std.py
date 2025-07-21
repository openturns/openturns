#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

f1 = ot.SymbolicFunction(
    ["a0", "a1"],
    ["-4 * exp((-25 / 8) * (a0^2 + a1^2)) + 7 * exp((-125 / 4) * (a0^2 + a1^2))"] * 2,
)

distribution = ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * 2)
x0 = ot.LowDiscrepancyExperiment(ot.HaltonSequence(), distribution, 100).generate()
y0 = f1(x0)

# reference mean for each generated batch of input x
ref_mean = [
    [-0.0271247, -0.0171091],
    [-0.0763905, 0.0570268],
    [0.0832077, -0.0235941],
    [0.0488792, 0.158224],
    [0.0511363, 0.113745],
]

algo = otexp.LOLAVoronoi(x0, y0, distribution)
newX = ot.Sample(0, x0.getDimension())
inc = 15
for i in range(5):
    x = algo.generate(inc)
    newX.add(x)
    y = f1(x)
    algo.update(x, y)

    x_mean = x.computeMean()
    print(f"iteration={i} x_mean={x_mean}")

    lolaScore = algo.getLOLAScore()
    voronoiScore = algo.getVoronoiScore()
    assert len(lolaScore) == len(x0) + i * inc
    assert len(lolaScore) == len(voronoiScore)

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

# linear model: the non linearity score should have no effect
ot.ResourceMap.SetAsString("LOLAVoronoi-NonLinearityAggregationMethod", "Maximum")
f2 = ot.SymbolicFunction(["x0", "x1"], ["2 * x0 + 3 * x1 + 8"])
y0 = f2(x0)
algo = otexp.LOLAVoronoi(x0, y0, distribution)
for i in range(10):
    x = algo.generate(20)
    y = f2(x)
    algo.update(x, y)
    x_mean = x.computeMean()
    print(f"iteration={i} x_mean={x_mean}")
ott.assert_almost_equal(algo.getLOLAScore().asPoint().norm(), 0.0)
X = algo.getInputSample()
discrepancy = ot.SobolSequence.ComputeStarDiscrepancy(X)
assert discrepancy < 1.0, "wrong repartition"

# oscillating model on a grid
ot.ResourceMap.SetAsUnsignedInteger("LOLAVoronoi-MaximumCombinationsNumber", 3870)
f3 = ot.SymbolicFunction(["x0", "x1"], ["sin(10 * x0) + cos(10 * x1)"])
x0 = ot.Box([8, 8]).generate()
y0 = f3(x0)
algo = otexp.LOLAVoronoi(x0, y0, distribution)
for i in range(10):
    x = algo.generate(20)
    y = f3(x)
    algo.update(x, y)
    x_mean = x.computeMean()
    print(f"iteration={i} x_mean={x_mean}")
X = algo.getInputSample()
discrepancy = ot.SobolSequence.ComputeStarDiscrepancy(X)
assert discrepancy < 1.0, "wrong repartition"
