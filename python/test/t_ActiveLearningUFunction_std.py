#! /usr/bin/env python

import os
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.RandomGenerator.SetSeed(0)

# 2-d linear model with a known failure threshold
model = ot.SymbolicFunction(["x1", "x2"], ["x1 + 2.0 * x2 - 3.0"])
distribution = ot.JointDistribution([ot.Normal(), ot.Normal()])
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
criterion = otexp.ActiveLearningUFunction(0.0, 2.0)
assert criterion.getReliabilityThreshold() == 0.0
assert criterion.getLearningThreshold() == 2.0
criterion.setGaussianProcessRegression(gprResult)
assert criterion.getInputDimension() == 2
assert criterion.getOutputDimension() == 1
assert "ActiveLearningUFunction" in criterion.__repr__()
assert not criterion.isMaximization()

criterion.setReliabilityThreshold(0.5)
assert criterion.getReliabilityThreshold() == 0.5
criterion.setReliabilityThreshold(0.0)
criterion.setLearningThreshold(3.0)
assert criterion.getLearningThreshold() == 3.0
criterion.setLearningThreshold(2.0)

# invalid thresholds raise
with ott.assert_raises(TypeError):
    otexp.ActiveLearningUFunction(float("nan"), 2.0)
with ott.assert_raises(TypeError):
    otexp.ActiveLearningUFunction(0.0, -1.0)
with ott.assert_raises(TypeError):
    criterion.setReliabilityThreshold(float("nan"))
with ott.assert_raises(TypeError):
    criterion.setLearningThreshold(-1.0)

# U is nonnegative and finite away from the design
point = [0.5, 0.5]
score = criterion([point], inputSample)
assert score[0][0] >= 0.0
assert score[0][0] < float("inf")

# point and sample evaluations agree
scores = criterion(inputSample, inputSample)
assert scores.getSize() == inputSample.getSize()
assert scores.getDimension() == 1
ott.assert_almost_equal(scores[1][0], criterion([inputSample[1]], inputSample)[0][0])

# U is huge on design points: the residual surrogate variance is never
# favorable to the minimization that selects the infill point
assert criterion([inputSample[0]], inputSample)[0][0] > 1.0e6

# the infill point minimizes U over the pool
pool = distribution.getSample(50)
poolScores = criterion(pool, inputSample)
infill = criterion.getInfillSample(pool, poolScores)
assert infill.getSize() == 1
ott.assert_almost_equal(infill[0], pool[poolScores.argsort(True)[0]])

# convergence: all safe candidates converge, unsafe ones do not
assert criterion.checkConvergenceLearning(ot.Sample([[2.5], [3.0]]))
assert not criterion.checkConvergenceLearning(ot.Sample([[2.5], [0.5]]))
with ott.assert_raises(TypeError):
    criterion.checkConvergenceLearning(ot.Sample(0, 1))

# update of the surrogate result
criterion.setGaussianProcessRegression(gprResult)
ott.assert_almost_equal(
    criterion.getGaussianProcessRegression().getMetaModel()(point),
    gprResult.getMetaModel()(point),
)

# copy preserves the thresholds
clone = otexp.ActiveLearningUFunction(criterion)
assert clone.getReliabilityThreshold() == 0.0
assert clone.getLearningThreshold() == 2.0

# persistence round-trip through a Study
fileName = "t_ActiveLearningUFunction_std.xml"
study = ot.Study(fileName)
study.add("criterion", criterion)
study.save()
study2 = ot.Study(fileName)
study2.load()
loaded = otexp.ActiveLearningUFunction()
study2.fillObject("criterion", loaded)
assert loaded.getReliabilityThreshold() == 0.0
assert loaded.getLearningThreshold() == 2.0
assert not loaded.isMaximization()
ott.assert_almost_equal(
    loaded(pool, inputSample), criterion(pool, inputSample)
)
os.remove(fileName)
