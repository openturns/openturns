#! /usr/bin/env python

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

# no absolute frame bounds at m = n: the projection DPP is still far more
# stable than a uniform design at the same budget, whose Gramian is
# numerically singular in a 30-function space
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
# zero and negative weights are rejected: the frame certificate would ignore them
with ott.assert_raises(TypeError):
    experiment.computeDesignEigenvalues(sample, ot.Point(30, 0.0))
with ott.assert_raises(TypeError):
    experiment.computeDesignEigenvalues(sample, ot.Point(30, -1.0))

# setters rebuild the derived members
experiment.setSize(50)
assert experiment.getSize() == 50
assert experiment.getSpaceDimension() == 50
experiment.setOrthogonalBasis(basis)
assert experiment.getSpaceDimension() == 50

# the generation is deterministic: same seed gives the same design
ot.RandomGenerator.SetSeed(0)
sampleA, weightsA = experiment.generateWithWeights()
ot.RandomGenerator.SetSeed(0)
sampleB, weightsB = experiment.generateWithWeights()
ott.assert_almost_equal(sampleA, sampleB)
ott.assert_almost_equal(weightsA, weightsB)

# the sizing rule needs no gamma key anymore: m = n-k is deduced by trial,
# and the pool, thinning and barrier controls are gone
assert not ot.ResourceMap.HasKey("ChristoffelSubsampleExperiment-Gamma")
for key in [
    "ChristoffelSubsampleExperiment-PoolOversamplingFactor",
    "ChristoffelSubsampleExperiment-PoolMaxOversamplingFactor",
    "ChristoffelSubsampleExperiment-ThinningMethod",
    "ChristoffelSubsampleExperiment-BarrierStep",
    "ChristoffelSubsampleExperiment-BarrierRegularization",
]:
    assert not ot.ResourceMap.HasKey(key)
ott.assert_almost_equal(ot.ResourceMap.GetAsScalar("ChristoffelSubsampleExperiment-FrameTolerance"), 0.5)
existing = reference.getSample(20)

# small space dimension (m=15): exact projection DPP, no pool
smallExperiment = otexp.ChristoffelSubsampleExperiment(basis, 15)
assert smallExperiment.getSpaceDimension() == 15
smallSample, smallWeights = smallExperiment.generateWithWeights()
assert smallSample.getSize() == 15
assert len(smallWeights) == 15
assert all(w > 0.0 for w in smallWeights)
assert len(set(tuple(point) for point in smallSample)) == 15

# m=2 exact DPP moments: E[(X-Y)^2] = 8/5 and E[X^2] = 7/15 for the
# projection DPP of the first two Legendre functions, against 2/3 and 1/3
# for independent uniform draws
twoExperiment = otexp.ChristoffelSubsampleExperiment(basis, 2)
squaredDifference = 0.0
square = 0.0
twoCount = 2000
for k in range(twoCount):
    ot.RandomGenerator.SetSeed(500000 + k)
    twoSample, twoWeights = twoExperiment.generateWithWeights()
    squaredDifference += (twoSample[0, 0] - twoSample[1, 0]) ** 2
    square += twoSample[0, 0] ** 2
squaredDifference /= twoCount
square /= twoCount
assert abs(squaredDifference - 1.6) < 0.12
assert abs(square - 7.0 / 15.0) < 0.05

# finite basis: k is deduced by trial, m = n-k supplied by the basis
finiteFunctions = [ot.SymbolicFunction("x", formula) for formula in ["1.0", "x", "x^2", "x^3", "x^4"]]
finiteFactory = otexp.FiniteOrthogonalFunctionFactory(finiteFunctions, ot.Uniform(-1.0, 1.0))
finiteBasis = ot.OrthogonalBasis(finiteFactory)
finiteExperiment = otexp.ChristoffelSubsampleExperiment(finiteBasis, 30)
assert finiteExperiment.getSpaceDimension() == 5
finiteSample, finiteWeights = finiteExperiment.generateWithWeights()
assert finiteSample.getSize() == finiteExperiment.getSize()
assert all(w > 0.0 for w in finiteWeights)

# reuse path with M >= n: exact volume sample of the supplied candidates
ot.RandomGenerator.SetSeed(1)
candidatePool = reference.getSample(100)
volumeSample, volumeWeights = experiment.generateWithWeights(candidatePool)
assert volumeSample.getSize() == 50
assert len(volumeWeights) == 50
assert all(w > 0.0 for w in volumeWeights)
assert len(set(tuple(point) for point in volumeSample)) == 50
candidateTuples = set(tuple(point) for point in candidatePool)
assert all(tuple(point) in candidateTuples for point in volumeSample)

# an empty supplied pool delegates to the fresh projection DPP path
ot.RandomGenerator.SetSeed(2)
freshSample, freshWeights = experiment.generateWithWeights()
ot.RandomGenerator.SetSeed(2)
emptySample, emptyWeights = experiment.generateWithWeights(ot.Sample(0, 1))
ott.assert_almost_equal(freshSample, emptySample)
ott.assert_almost_equal(freshWeights, emptyWeights)

# M < n and M <= m: conditional projection DPP anchored on every supplied point
ot.RandomGenerator.SetSeed(3)
conditionalSample, conditionalWeights = experiment.generateWithWeights(existing)
assert conditionalSample.getSize() == 50
assert len(conditionalWeights) == 50
assert all(w > 0.0 for w in conditionalWeights)
ott.assert_almost_equal(conditionalSample[:20], existing)

# M < n but M > m: no projection DPP can be conditioned on that many points
with ott.assert_raises(TypeError):
    finiteExperiment.generateWithWeights(reference.getSample(10))

# M <= m on a larger target: conditional DPP block plus independent fill
ot.RandomGenerator.SetSeed(4)
smallPool = reference.getSample(3)
filledSample, filledWeights = finiteExperiment.generateWithWeights(smallPool)
assert filledSample.getSize() == 30
assert len(filledWeights) == 30
assert all(w > 0.0 for w in filledWeights)
ott.assert_almost_equal(filledSample[:3], smallPool)

# dependent supplied features cannot anchor a conditional DPP
with ott.assert_raises(TypeError):
    experiment.generateWithWeights(ot.Sample(2, [0.5]))

# an out-of-support supplied point has a zero importance weight
with ott.assert_raises(RuntimeError):
    twoExperiment.generateWithWeights(ot.Sample([[0.5], [3.0]]))

# wrong supplied dimension raises
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
with ott.assert_raises(TypeError):
    defaultExperiment.generateWithWeights()
with ott.assert_raises(TypeError):
    defaultExperiment.generate()

# a negative frame tolerance is rejected
ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-FrameTolerance", -1.0)
try:
    with ott.assert_raises(TypeError):
        experiment.generateWithWeights()
finally:
    ot.ResourceMap.SetAsScalar("ChristoffelSubsampleExperiment-FrameTolerance", 0.5)
