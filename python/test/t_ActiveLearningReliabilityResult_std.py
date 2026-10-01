#! /usr/bin/env python

import os
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.RandomGenerator.SetSeed(0)

# surrogate result to embed, from a small regression
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

probability = 0.09
index = -ot.Normal().computeCDF(probability)
probabilityHistory = [0.12, 0.10, probability]
indexHistory = [-ot.Normal().computeCDF(p) for p in probabilityHistory]
probabilityCI = ot.Interval([0.07], [0.11])
indexCI = ot.Interval(
    [-ot.Normal().computeCDF(0.07)], [-ot.Normal().computeCDF(0.11)]
)

# constructor and accessors
result = otexp.ActiveLearningReliabilityResult(
    probability,
    index,
    gprResult,
    probabilityHistory,
    indexHistory,
    6,
    probabilityCI,
    indexCI,
)
ott.assert_almost_equal(result.getProbabilityEstimate(), probability)
ott.assert_almost_equal(result.getReliabilityIndex(), index)
history = result.getProbabilityHistory()
assert len(history) == len(probabilityHistory)
for i in range(len(history)):
    ott.assert_almost_equal(history[i], probabilityHistory[i])
indexHistoryResult = result.getReliabilityIndexHistory()
assert len(indexHistoryResult) == len(indexHistory)
for i in range(len(indexHistoryResult)):
    ott.assert_almost_equal(indexHistoryResult[i], indexHistory[i])
assert result.getFunctionCallNumber() == 6
assert not result.getHasConverged()
result.setHasConverged(True)
assert result.getHasConverged()
ott.assert_almost_equal(
    result.getProbabilityConfidenceInterval().getLowerBound(),
    probabilityCI.getLowerBound(),
)
ott.assert_almost_equal(
    result.getProbabilityConfidenceInterval().getUpperBound(),
    probabilityCI.getUpperBound(),
)
ott.assert_almost_equal(
    result.getReliabilityIndexConfidenceInterval().getLowerBound(),
    indexCI.getLowerBound(),
)
ott.assert_almost_equal(
    result.getReliabilityIndexConfidenceInterval().getUpperBound(),
    indexCI.getUpperBound(),
)
assert "ActiveLearningReliabilityResult" in result.__repr__()

# setters update the stored values
result.setProbabilityEstimate(0.10)
ott.assert_almost_equal(result.getProbabilityEstimate(), 0.10)
result.setReliabilityIndex(1.28)
ott.assert_almost_equal(result.getReliabilityIndex(), 1.28)
result.setFunctionCallNumber(7)
assert result.getFunctionCallNumber() == 7
result.setProbabilityHistory([0.10])
history = result.getProbabilityHistory()
assert len(history) == 1
ott.assert_almost_equal(history[0], 0.10)

# copy preserves the content
clone = otexp.ActiveLearningReliabilityResult(result)
ott.assert_almost_equal(clone.getProbabilityEstimate(), 0.10)
assert clone.getFunctionCallNumber() == 7

# persistence round-trip through a Study
fileName = "t_ActiveLearningReliabilityResult_std.xml"
study = ot.Study(fileName)
study.add("result", result)
study.save()
study2 = ot.Study(fileName)
study2.load()
loaded = otexp.ActiveLearningReliabilityResult()
study2.fillObject("result", loaded)
ott.assert_almost_equal(loaded.getProbabilityEstimate(), 0.10)
assert loaded.getFunctionCallNumber() == 7
loadedHistory = loaded.getProbabilityHistory()
assert len(loadedHistory) == 1
ott.assert_almost_equal(loadedHistory[0], 0.10)
ott.assert_almost_equal(
    loaded.getProbabilityConfidenceInterval().getLowerBound(),
    probabilityCI.getLowerBound(),
)
os.remove(fileName)
