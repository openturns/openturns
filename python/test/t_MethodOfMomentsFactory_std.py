#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# need a proper LS solver
if not ot.PlatformInfo.HasFeature("cminpack") or not ot.PlatformInfo.HasFeature("ceres"):
    exit(0)

size = 10000

distribution = ot.Gumbel(1.5, -0.5)
print("distribution=", distribution)
sample = distribution.getSample(size)
factory = ot.MethodOfMomentsFactory(ot.Gumbel(), [1, 2])
inf_dist = factory.build(sample)
print("estimated distribution=", inf_dist)
p_ref = [1.50988, -0.481183]
ott.assert_almost_equal(inf_dist.getParameter(), p_ref, 1e-2, 1e-2)

# set (a,b) out of (r, t, a, b)
distribution = ot.Beta(2.3, 2.2, -1.0, 1.0)
print("distribution=", distribution)
sample = distribution.getSample(size)
factory = ot.MethodOfMomentsFactory(ot.Beta(), [1, 2])
factory.setKnownParameter([-1.0, 1.0], [2, 3])
inf_dist = factory.build(sample)
print("estimated distribution=", inf_dist)
p_ref = [2.27806, 2.20053, -1, 1]
ott.assert_almost_equal(inf_dist.getParameter(), p_ref, 1e-2, 1e-2)

# from moments
distribution = ot.Beta(2.3, 2.2, -1.0, 1.0)
factory = ot.MethodOfMomentsFactory(ot.Beta(), [1, 2, 3, 4])
cm = [distribution.getCentralMoment(i + 2)[0] for i in range(3)]
moments = [distribution.getMean()[0]] + cm
inf_dist = factory.buildFromMoments(moments)
print("estimated distribution (moments)=", inf_dist)
p_ref = [2.3, 2.2, -1, 1]
ott.assert_almost_equal(inf_dist.getParameter(), p_ref, 1e-2, 1e-2)

# with bounds
data = [
    0.6852,
    0.9349,
    0.5884,
    1.727,
    1.581,
    0.3193,
    -0.5701,
    1.623,
    2.210,
    -0.3440,
    -0.1646,
]
sample = ot.Sample([[x] for x in data])
size = sample.getSize()
xMin = sample.getMin()[0]
xMax = sample.getMax()[0]
delta = xMax - xMin
a = xMin - delta / (size + 2)
b = xMax + delta / (size + 2)
distribution = ot.TruncatedNormal()
factory = ot.MethodOfMomentsFactory(distribution, [1, 2, 3, 4])
factory.setKnownParameter([a, b], [2, 3])
solver = factory.getOptimizationAlgorithm()
sampleMean = sample.computeMean()[0]
sampleSigma = sample.computeStandardDeviation()[0]
startingPoint = [sampleMean, sampleSigma]
solver.setStartingPoint(startingPoint)
factory.setOptimizationAlgorithm(solver)
lowerBound = [-1.0, 0]
upperBound = [-1.0, 1.5]
finiteLowerBound = [False, True]
finiteUpperBound = [False, True]
bounds = ot.Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound)
factory = ot.MethodOfMomentsFactory(distribution, [3, 4], bounds)
factory.setKnownParameter([a, b], [2, 3])
factory.setOptimizationBounds(bounds)
inf_dist = factory.build(sample)
print("estimated distribution=", inf_dist)
p_ref = [0.805158, 1.5, -0.783954, 2.42385]
ott.assert_almost_equal(inf_dist.getParameter(), p_ref, 1e-2, 1e-2)

# setKnownParameter+buildEstimator
sample = ot.Normal(2.0, 1.0).getSample(size)
factory = ot.MethodOfMomentsFactory(ot.Normal(), [1])
factory.setBootstrapSize(4)
factory.setKnownParameter([1.0], [1])  # set the sigma parameter to 1.0
result = factory.buildEstimator(sample)
inf_dist = result.getDistribution().getParameter()
print("estimated distribution=", inf_dist)
p_ref = [2.04553, 1.0]
ott.assert_almost_equal(result.getDistribution().getParameter(), p_ref, 1e-3, 1e-3)
