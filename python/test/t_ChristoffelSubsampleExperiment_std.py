#! /usr/bin/env python

import math
import pickle
from io import BytesIO

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

factory = ot.OrthogonalProductPolynomialFactory([ot.Uniform(-1.0, 1.0)])
basis = ot.OrthogonalBasis(factory)

# sizing rule: largest m with gamma*m*log(m) <= n (gamma = 9.242343873386666)
assert otexp.ChristoffelSubsampleExperiment.DeduceSpaceDimension(0) == 1
assert otexp.ChristoffelSubsampleExperiment.DeduceSpaceDimension(10) == 1
assert otexp.ChristoffelSubsampleExperiment.DeduceSpaceDimension(60) == 4
assert otexp.ChristoffelSubsampleExperiment.DeduceSpaceDimension(100) == 6

experiment = otexp.ChristoffelSubsampleExperiment(basis, 60)
assert experiment.getSize() == 60
assert experiment.getSpaceDimension() == 4
assert experiment.getDistribution().getDimension() == 1
assert not experiment.hasUniformWeights()
assert experiment.isRandom()
assert "ChristoffelSubsampleExperiment" in repr(experiment)

# weighted sample: n points in the range, positive weights of mean about 1
sample, weights = experiment.generateWithWeights()
assert sample.getSize() == 60
assert len(weights) == 60
assert all(w > 0.0 for w in weights)
assert abs(sum(weights) - 60.0) < 3.0
for point in sample:
    assert -1.0 <= point[0] <= 1.0

# default Removal: weights are exactly the density ratios, no reweighting
reference = experiment.getDistribution()
christoffel = otexp.ChristoffelDistribution(basis, 4)
ott.assert_almost_equal(weights[0], reference.computePDF(sample[0]) / christoffel.computePDF(sample[0]))

# the thinned design satisfies the frame bounds, read off the class itself
eigenvalues = experiment.computeDesignEigenvalues(sample, weights)
assert all(0.5 <= value <= 1.5 for value in eigenvalues)
with ott.assert_raises(TypeError):
    experiment.computeDesignEigenvalues(ot.Sample(0, 1), ot.Point(0))
with ott.assert_raises(TypeError):
    experiment.computeDesignEigenvalues(ot.Sample(5, 2), ot.Point(5, 1.0))
with ott.assert_raises(TypeError):
    experiment.computeDesignEigenvalues(sample, ot.Point(3, 1.0))

# setters rebuild the derived members
experiment.setSize(100)
assert experiment.getSize() == 100
assert experiment.getSpaceDimension() == 6
experiment.setOrthogonalBasis(basis)
assert experiment.getSpaceDimension() == 6

# the thinning is deterministic: same seed gives the same design
ot.RandomGenerator.SetSeed(0)
sampleA, weightsA = experiment.generateWithWeights()
ot.RandomGenerator.SetSeed(0)
sampleB, weightsB = experiment.generateWithWeights()
ott.assert_almost_equal(sampleA, sampleB)
ott.assert_almost_equal(weightsA, weightsB)

# the sampling factor key drives the sizing rule
ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-Gamma", 5.0)
try:
    assert otexp.ChristoffelSubsampleExperiment.DeduceSpaceDimension(60) == 6
finally:
    ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-Gamma", 9.242343873386666)
ott.assert_almost_equal(ot.ResourceMap.GetAsScalar("ChristoffelSubsampleExperiment-Gamma"), 9.242343873386666)
ott.assert_almost_equal(ot.ResourceMap.GetAsScalar("ChristoffelSubsampleExperiment-PoolOversamplingFactor"), 2.0)
ott.assert_almost_equal(ot.ResourceMap.GetAsScalar("ChristoffelSubsampleExperiment-FrameTolerance"), 0.5)

# Barrier method: non-trivial reweighting, smoke only (no frame bounds)
ot.ResourceMap.SetAsString("ChristoffelSubsampleExperiment-ThinningMethod", "Barrier")
try:
    barrierSample, barrierWeights = experiment.generateWithWeights()
    assert barrierSample.getSize() == 100
    assert len(barrierWeights) == 100
    assert all(w > 0.0 for w in barrierWeights)
finally:
    ot.ResourceMap.SetAsString("ChristoffelSubsampleExperiment-ThinningMethod", "Removal")

# small space dimension (m=2): barrier schedule stays inside the spectrum
smallExperiment = otexp.ChristoffelSubsampleExperiment(basis, 30)
assert smallExperiment.getSpaceDimension() == 2
smallSample, smallWeights = smallExperiment.generateWithWeights()
assert smallSample.getSize() == 30
assert len(smallWeights) == 30
assert all(w > 0.0 for w in smallWeights)

# pool factor 1 fast path: no thinning, direct Christoffel draws
ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-PoolOversamplingFactor", 1.0)
try:
    directSample, directWeights = experiment.generateWithWeights()
    assert directSample.getSize() == 100
    assert len(directWeights) == 100
finally:
    ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-PoolOversamplingFactor", 2.0)

# pool input: existing points are prepended, fresh draws top up
existing = reference.getSample(20)
ot.RandomGenerator.SetSeed(1)
pooledSample, pooledWeights = experiment.generateWithWeights(existing)
assert pooledSample.getSize() == 100
assert len(pooledWeights) == 100
assert all(w > 0.0 for w in pooledWeights)
ot.RandomGenerator.SetSeed(1)
pooledSample2, pooledWeights2 = experiment.generateWithWeights(existing)
ott.assert_almost_equal(pooledSample, pooledSample2)
ott.assert_almost_equal(pooledWeights, pooledWeights2)
# empty pool input behaves like the plain call
ot.RandomGenerator.SetSeed(2)
plainSample, plainWeights = experiment.generateWithWeights()
ot.RandomGenerator.SetSeed(2)
emptyPooledSample, emptyPooledWeights = experiment.generateWithWeights(ot.Sample(0, 1))
ott.assert_almost_equal(plainSample, emptyPooledSample)
ott.assert_almost_equal(plainWeights, emptyPooledWeights)
# wrong pool dimension raises
with ott.assert_raises(TypeError):
    experiment.generateWithWeights(ot.Sample(5, 2))

# clone, comparison and persistence
assert experiment == otexp.ChristoffelSubsampleExperiment(basis, 100)
assert experiment != otexp.ChristoffelSubsampleExperiment(basis, 60)
buf = BytesIO()
pickle.dump(experiment, buf)
buf.seek(0)
assert pickle.load(buf) == experiment

# errors: the reference distribution is embedded in the basis
with ott.assert_raises(TypeError):
    experiment.setDistribution(ot.Normal())
with ott.assert_raises(TypeError):
    otexp.ChristoffelSubsampleExperiment(basis, 0)
with ott.assert_raises(TypeError):
    experiment.setSize(0)

# accessors and default state: the base default size, no deduced space
# dimension until setOrthogonalBasis() and setSize() are called
assert experiment.getOrthogonalBasis() == basis
defaultExperiment = otexp.ChristoffelSubsampleExperiment()
assert defaultExperiment.getSize() == ot.ResourceMap.GetAsUnsignedInteger("WeightedExperiment-DefaultSize")
assert defaultExperiment.getSpaceDimension() == 0

# a pool point outside the numerical range has a null law density: its
# importance ratio is undefined and it must not divide by zero
mixedPool = ot.Sample([[0.5], [3.0], [-0.7]])
mixedSample, mixedWeights = experiment.generateWithWeights(mixedPool)
assert mixedSample.getSize() == 100
assert all(math.isfinite(w) for w in mixedWeights)

# the sizing rule rejects a non-positive gamma
ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-Gamma", 0.0)
try:
    with ott.assert_raises(TypeError):
        otexp.ChristoffelSubsampleExperiment.DeduceSpaceDimension(60)
finally:
    ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-Gamma", 9.242343873386666)

# a pool oversampling factor below 1 is rejected
ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-PoolOversamplingFactor", 0.5)
try:
    with ott.assert_raises(TypeError):
        experiment.generateWithWeights()
finally:
    ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-PoolOversamplingFactor", 2.0)

# the barrier schedule rejects non-positive controls
ot.ResourceMap.SetAsString("ChristoffelSubsampleExperiment-ThinningMethod", "Barrier")
ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-BarrierStep", 0.0)
try:
    with ott.assert_raises(TypeError):
        experiment.generateWithWeights()
finally:
    ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-BarrierStep", 1.0)
    ot.ResourceMap.SetAsString("ChristoffelSubsampleExperiment-ThinningMethod", "Removal")
ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-BarrierRegularization", 0.0)
ot.ResourceMap.SetAsString("ChristoffelSubsampleExperiment-ThinningMethod", "Barrier")
try:
    with ott.assert_raises(TypeError):
        experiment.generateWithWeights()
finally:
    ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-BarrierRegularization", 1e-8)
    ot.ResourceMap.SetAsString("ChristoffelSubsampleExperiment-ThinningMethod", "Removal")
