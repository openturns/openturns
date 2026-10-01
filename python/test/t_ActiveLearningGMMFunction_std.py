#! /usr/bin/env python

import math
import os
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.RandomGenerator.SetSeed(0)

# 2-d linear model with a known failure threshold
model = ot.SymbolicFunction(["x1", "x2"], ["x1 + 2.0 * x2 - 3.0"])
distribution = ot.JointDistribution([ot.Normal(), ot.Normal()])
threshold = 0.0
inputSample = distribution.getSample(12)
outputSample = model(inputSample)
covarianceModel = ot.SquaredExponential([1.0] * 2, [1.0])
basis = ot.ConstantBasisFactory(2).build()
fitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
fitter.run()
regressor = ot.GaussianProcessRegression(fitter.getResult())
regressor.run()
gprResult = regressor.getResult()

# constructor and accessors
criterion = otexp.ActiveLearningGMMFunction(threshold, 0.1, distribution, 0.5)
assert criterion.getReliabilityThreshold() == threshold
assert criterion.getLearningThreshold() == 0.1
criterion.setGaussianProcessRegression(gprResult)
assert criterion.getInputDimension() == 2
assert criterion.getOutputDimension() == 1
assert "ActiveLearningGMMFunction" in criterion.__repr__()
assert criterion.isMaximization()

# invalid quantile level raises
with ott.assert_raises(TypeError):
    otexp.ActiveLearningGMMFunction(threshold, 0.1, distribution, -0.1)
with ott.assert_raises(TypeError):
    otexp.ActiveLearningGMMFunction(threshold, 0.1, distribution, 1.5)
with ott.assert_raises(TypeError):
    otexp.ActiveLearningGMMFunction(float("nan"), 0.1, distribution, 0.5)

# scores are nonnegative distances, zero far from the limit state
pool = distribution.getSample(60)
scores = criterion(pool, inputSample)
assert scores.getSize() == pool.getSize()
for i in range(pool.getSize()):
    assert scores[i][0] >= 0.0
# the quantile splits the pool: points far from the limit state score zero,
# points close to it score the weighted distance to the design
hasZero = any(scores[i][0] == 0.0 for i in range(pool.getSize()))
hasPositive = any(scores[i][0] > 0.0 for i in range(pool.getSize()))
assert hasZero and hasPositive

# an empty design is rejected: no distance can be computed
with ott.assert_raises(TypeError):
    criterion(pool, ot.Sample(0, 2))
with ott.assert_raises(TypeError):
    criterion(pool)

# independent regression: hand recomputation of one score from the
# surrogate prediction only (quantile by sorting with the documented
# p*n-0.5 linear rule, density by hand, distances by hand). The
# criterion is pool-relative by design: the quantile prefilter splits
# the evaluated sample, so scores are checked on a full-pool evaluation.
probeIndex = 3
probe = pool[probeIndex]
conditionalCovariance = ot.GaussianProcessConditionalCovariance(gprResult)
mean = conditionalCovariance.getConditionalMean(pool)
scaled = [abs(mean[i][0] - threshold) for i in range(pool.getSize())]
ordered = sorted(scaled)
size = len(ordered)
scalarIndex = 0.5 * size - 0.5
if scalarIndex >= size - 1:
    expectedQuantile = ordered[-1]
elif scalarIndex <= 0.0:
    expectedQuantile = ordered[0]
else:
    index = math.floor(scalarIndex)
    beta = scalarIndex - index
    expectedQuantile = (1.0 - beta) * ordered[index] + beta * ordered[index + 1]
if scaled[probeIndex] > expectedQuantile:
    expected = 0.0
else:
    dimension = pool.getDimension()
    density = math.exp(-0.5 * (probe[0] ** 2 + probe[1] ** 2)) / (2.0 * math.pi)
    expected = (
        min(
            math.sqrt(
                (probe[0] - inputSample[j][0]) ** 2
                + (probe[1] - inputSample[j][1]) ** 2
            )
            for j in range(inputSample.getSize())
        )
        * density ** (1.0 / dimension)
    )
ott.assert_almost_equal(
    scores[probeIndex][0], expected, 1.0e-12, 1.0e-12
)

# the infill point maximizes the criterion over the pool
infill = criterion.getInfillSample(pool, scores)
assert infill.getSize() == 1
ott.assert_almost_equal(infill[0], pool[scores.argsort(False)[0]])

# convergence: small distances everywhere converge, large ones do not
assert criterion.checkConvergenceLearning(ot.Sample([[0.05], [0.01]]))
assert not criterion.checkConvergenceLearning(ot.Sample([[0.05], [0.5]]))
with ott.assert_raises(TypeError):
    criterion.checkConvergenceLearning(ot.Sample(0, 1))

# copy preserves the parameters
clone = otexp.ActiveLearningGMMFunction(criterion)
assert clone.getReliabilityThreshold() == threshold
assert clone.getLearningThreshold() == 0.1

# persistence round-trip through a Study, including the distribution
fileName = "t_ActiveLearningGMMFunction_std.xml"
study = ot.Study(fileName)
study.add("criterion", criterion)
study.save()
study2 = ot.Study(fileName)
study2.load()
loaded = otexp.ActiveLearningGMMFunction()
study2.fillObject("criterion", loaded)
assert loaded.getReliabilityThreshold() == threshold
assert loaded.getLearningThreshold() == 0.1
ott.assert_almost_equal(
    loaded(pool, inputSample), criterion(pool, inputSample)
)
os.remove(fileName)

# end-to-end enrichment with the GMM criterion on a small budget
eventGMM = ot.ThresholdEvent(
    ot.CompositeRandomVector(model, ot.RandomVector(distribution)),
    ot.Greater(),
    threshold,
)
simGMM = ot.ProbabilitySimulationAlgorithm(eventGMM)
simGMM.setMaximumOuterSampling(500)
simGMM.setBlockSize(100)
algoGMM = otexp.ActiveLearningReliabilityAlgorithm(fitter, simGMM, criterion)
algoGMM.setMaximumIterations(2)
algoGMM.setSimulationAlgorithmSeed(0)
algoGMM.run()
callsGMM = algoGMM.getFunctionCallNumber()
assert 0 <= callsGMM <= 2
assert algoGMM.getInputDoE().getSize() == inputSample.getSize() + callsGMM
probabilityGMM = algoGMM.getResult().getProbabilityEstimate()
print("GMM probability=", probabilityGMM)
assert 0.0 < probabilityGMM < 0.3
