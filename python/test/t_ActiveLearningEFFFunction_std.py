#! /usr/bin/env python

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
criterion = otexp.ActiveLearningEFFFunction(threshold, 0.01)
assert criterion.getReliabilityThreshold() == threshold
assert criterion.getLearningThreshold() == 0.01
criterion.setGaussianProcessRegression(gprResult)
assert criterion.getInputDimension() == 2
assert criterion.getOutputDimension() == 1
assert "ActiveLearningEFFFunction" in criterion.__repr__()
assert criterion.isMaximization()

# invalid thresholds raise
with ott.assert_raises(TypeError):
    otexp.ActiveLearningEFFFunction(float("nan"), 0.01)
with ott.assert_raises(TypeError):
    otexp.ActiveLearningEFFFunction(threshold, -1.0)

# independent closed form: the implementation must match Bichon et al.,
# including the epsilon factor of the last term
normal = ot.Normal()
point = [0.5, 0.5]
# conditional moments from an independent prediction
prediction = gprResult.getMetaModel()(point)
conditionalVariance = ot.GaussianProcessConditionalCovariance(gprResult).getConditionalMarginalVariance(
    point
)
mx = prediction[0]
sk = conditionalVariance**0.5
assert sk > 0.0
epsilon = 2.0 * sk
z = (threshold - mx) / sk
zMinus = (threshold - epsilon - mx) / sk
zPlus = (threshold + epsilon - mx) / sk
expected = (
    (mx - threshold)
    * (2.0 * normal.computeCDF(z) - normal.computeCDF(zPlus) - normal.computeCDF(zMinus))
    - sk
    * (2.0 * normal.computePDF(z) - normal.computePDF(zPlus) - normal.computePDF(zMinus))
    + epsilon * (normal.computeCDF(zPlus) - normal.computeCDF(zMinus))
)
ott.assert_almost_equal(criterion([point], inputSample)[0][0], expected, 1.0e-12, 1.0e-12)

# EFF is an expectation of a nonnegative improvement
pool = distribution.getSample(50)
scores = criterion(pool, inputSample)
assert scores.getSize() == pool.getSize()
for i in range(pool.getSize()):
    assert scores[i][0] >= 0.0

# EFF vanishes on design points: no expected improvement there
assert criterion([inputSample[0]], inputSample)[0][0] == 0.0

# the infill point maximizes EFF over the pool
infill = criterion.getInfillSample(pool, scores)
assert infill.getSize() == 1
ott.assert_almost_equal(infill[0], pool[scores.argsort(False)[0]])

# convergence: small improvements everywhere converge, large ones do not
assert criterion.checkConvergenceLearning(ot.Sample([[0.005], [0.001]]))
assert not criterion.checkConvergenceLearning(ot.Sample([[0.005], [0.5]]))
with ott.assert_raises(TypeError):
    criterion.checkConvergenceLearning(ot.Sample(0, 1))

# copy preserves the thresholds
clone = otexp.ActiveLearningEFFFunction(criterion)
assert clone.getReliabilityThreshold() == threshold
assert clone.getLearningThreshold() == 0.01

# persistence round-trip through a Study
fileName = "t_ActiveLearningEFFFunction_std.xml"
study = ot.Study(fileName)
study.add("criterion", criterion)
study.save()
study2 = ot.Study(fileName)
study2.load()
loaded = otexp.ActiveLearningEFFFunction()
study2.fillObject("criterion", loaded)
assert loaded.getReliabilityThreshold() == threshold
assert loaded.getLearningThreshold() == 0.01
ott.assert_almost_equal(
    loaded(pool, inputSample), criterion(pool, inputSample)
)
os.remove(fileName)
