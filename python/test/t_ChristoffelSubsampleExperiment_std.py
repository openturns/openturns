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

# sizing rule: m = n-k, largest dimension supplied by the basis (k = 0 here)
assert otexp.ChristoffelSubsampleExperiment.DeduceSpaceDimension(0) == 0
assert otexp.ChristoffelSubsampleExperiment.DeduceSpaceDimension(10) == 10
assert otexp.ChristoffelSubsampleExperiment.DeduceSpaceDimension(30) == 30
assert otexp.ChristoffelSubsampleExperiment.DeduceSpaceDimension(50) == 50

experiment = otexp.ChristoffelSubsampleExperiment(basis, 30)
assert experiment.getSize() == 30
assert experiment.getSpaceDimension() == 30
assert experiment.getDistribution().getDimension() == 1
assert not experiment.hasUniformWeights()
assert experiment.isRandom()
assert "ChristoffelSubsampleExperiment" in repr(experiment)

# weighted sample: n points in the range, positive weights of mean about 1
sample, weights = experiment.generateWithWeights()
assert sample.getSize() == 30
assert len(weights) == 30
assert all(w > 0.0 for w in weights)
assert abs(sum(weights) - 30.0) < 3.0
for point in sample:
    assert -1.0 <= point[0] <= 1.0

# default Removal: weights are exactly the density ratios, no reweighting
reference = experiment.getDistribution()
christoffel = otexp.ChristoffelDistribution(basis, 30)
ott.assert_almost_equal(weights[0], reference.computePDF(sample[0]) / christoffel.computePDF(sample[0]))

# no absolute frame bounds at m = n with a 2x pool: the thinned design is
# still far more stable than a uniform design at the same budget, whose
# Gramian is numerically singular in a 30-function space
eigenvalues = experiment.computeDesignEigenvalues(sample, weights)
ot.RandomGenerator.SetSeed(1)
uniformEigenvalues = experiment.computeDesignEigenvalues(basis.getMeasure().getSample(30), ot.Point(30, 1.0))
assert min(eigenvalues) > max(0.0, min(uniformEigenvalues))
with ott.assert_raises(TypeError):
    experiment.computeDesignEigenvalues(ot.Sample(0, 1), ot.Point(0))
with ott.assert_raises(TypeError):
    experiment.computeDesignEigenvalues(ot.Sample(5, 2), ot.Point(5, 1.0))
with ott.assert_raises(TypeError):
    experiment.computeDesignEigenvalues(sample, ot.Point(3, 1.0))

# setters rebuild the derived members
experiment.setSize(50)
assert experiment.getSize() == 50
assert experiment.getSpaceDimension() == 50
experiment.setOrthogonalBasis(basis)
assert experiment.getSpaceDimension() == 50

# the thinning is deterministic: same seed gives the same design
ot.RandomGenerator.SetSeed(0)
sampleA, weightsA = experiment.generateWithWeights()
ot.RandomGenerator.SetSeed(0)
sampleB, weightsB = experiment.generateWithWeights()
ott.assert_almost_equal(sampleA, sampleB)
ott.assert_almost_equal(weightsA, weightsB)

# the sizing rule needs no gamma key anymore: m = n-k is deduced by trial
assert not ot.ResourceMap.HasKey("ChristoffelSubsampleExperiment-Gamma")
ott.assert_almost_equal(ot.ResourceMap.GetAsScalar("ChristoffelSubsampleExperiment-PoolOversamplingFactor"), 2.0)
ott.assert_almost_equal(ot.ResourceMap.GetAsScalar("ChristoffelSubsampleExperiment-FrameTolerance"), 0.5)

# Barrier method: non-trivial reweighting, smoke only (no frame bounds)
ot.ResourceMap.SetAsString("ChristoffelSubsampleExperiment-ThinningMethod", "Barrier")
try:
    barrierSample, barrierWeights = experiment.generateWithWeights()
    assert barrierSample.getSize() == 50
    assert len(barrierWeights) == 50
    assert all(w > 0.0 for w in barrierWeights)
finally:
    ot.ResourceMap.SetAsString("ChristoffelSubsampleExperiment-ThinningMethod", "Removal")

# small space dimension (m=15): barrier schedule stays inside the spectrum
smallExperiment = otexp.ChristoffelSubsampleExperiment(basis, 15)
assert smallExperiment.getSpaceDimension() == 15
smallSample, smallWeights = smallExperiment.generateWithWeights()
assert smallSample.getSize() == 15
assert len(smallWeights) == 15
assert all(w > 0.0 for w in smallWeights)

# finite basis: k is deduced by trial, m = n-k supplied by the basis
finiteFunctions = [ot.SymbolicFunction("x", formula) for formula in ["1.0", "x", "x^2", "x^3", "x^4"]]
finiteFactory = otexp.FiniteOrthogonalFunctionFactory(finiteFunctions, ot.Uniform(-1.0, 1.0))
finiteBasis = ot.OrthogonalBasis(finiteFactory)
finiteExperiment = otexp.ChristoffelSubsampleExperiment(finiteBasis, 30)
assert finiteExperiment.getSpaceDimension() == 5

# pool factor 1 fast path: no thinning, direct Christoffel draws
ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-PoolOversamplingFactor", 1.0)
try:
    directSample, directWeights = experiment.generateWithWeights()
    assert directSample.getSize() == 50
    assert len(directWeights) == 50
finally:
    ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-PoolOversamplingFactor", 2.0)

# pool input: existing points are prepended, fresh draws top up
existing = reference.getSample(20)
ot.RandomGenerator.SetSeed(1)
pooledSample, pooledWeights = experiment.generateWithWeights(existing)
assert pooledSample.getSize() == 50
assert len(pooledWeights) == 50
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
assert experiment == otexp.ChristoffelSubsampleExperiment(basis, 50)
assert experiment != otexp.ChristoffelSubsampleExperiment(basis, 30)
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
assert mixedSample.getSize() == 50
assert all(math.isfinite(w) for w in mixedWeights)

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
