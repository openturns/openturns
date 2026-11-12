#! /usr/bin/env python

import os
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

# A file-backed Study needs a libxml2-enabled build
HAVE_LIBXML2 = ot.PlatformInfo.HasFeature("libxml2")

ot.RandomGenerator.SetSeed(0)

# 2-d linear limit state with analytic failure probability
# g(X) = X1 + 2 X2, Pf = P(g > 3) = 1 - Phi(3 / sqrt(5))
model = ot.SymbolicFunction(["x1", "x2"], ["x1 + 2.0 * x2"])
distribution = ot.JointDistribution([ot.Normal(), ot.Normal()])
threshold = 3.0
reference = 1.0 - ot.Normal().computeCDF(threshold / 5.0**0.5)

Y = ot.CompositeRandomVector(model, ot.RandomVector(distribution))
event = ot.ThresholdEvent(Y, ot.Greater(), threshold)

# initial design of experiment, wrapped in a parameterized fitter
initialSize = 8
inputSample = distribution.getSample(initialSize)
outputSample = model(inputSample)
covarianceModel = ot.SquaredExponential([1.0] * 2, [1.0])
basis = ot.ConstantBasisFactory(2).build()
fitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)

# wrapped simulation on the surrogate event, with a fixed seed
simulation = ot.ProbabilitySimulationAlgorithm(event)
simulation.setMaximumOuterSampling(1000)
# A reasonably tight coefficient of variation keeps the estimate of the
# surrogate event accurate enough to compare with the analytic reference
# while keeping the test fast
simulation.setMaximumCoefficientOfVariation(0.04)
simulation.setBlockSize(100)

budget = 20
criterion = otexp.ActiveLearningUFunction(threshold, 2.0)
assert not criterion.isMaximization()
algo = otexp.ActiveLearningReliabilityAlgorithm(fitter, simulation, criterion)
algo.setMaximumIterations(budget)
algo.setConvergenceCriterion(algo.ACTIVE_LEARNING)
algo.setSimulationAlgorithmSeed(0)
assert algo.getMaximumIterations() == budget
assert "ActiveLearningReliabilityAlgorithm" in algo.__repr__()

# the design, surrogate specification and event are sourced from the
# fitter and the simulation
ott.assert_almost_equal(algo.getInputDoE(), inputSample)
ott.assert_almost_equal(algo.getOutputDoE(), outputSample)
assert algo.getCovarianceModel().getInputDimension() == 2
assert algo.getBasis().getSize() == basis.getSize()
assert algo.getEvent().getThreshold() == threshold

# invalid designs raise
with ott.assert_raises(TypeError):
    otexp.ActiveLearningReliabilityAlgorithm(
        ot.GaussianProcessFitter(), simulation, criterion
    )

# invalid settings raise
with ott.assert_raises(TypeError):
    algo.setConvergenceCriterion(5)
with ott.assert_raises(TypeError):
    algo.setMaximumIterations(0)
with ott.assert_raises(TypeError):
    algo.setConvergenceCriterionThreshold(-1.0)
with ott.assert_raises(TypeError):
    algo.setConvergenceUncertaintyFactor(-1.0)

# run the enrichment loop
algo.run()
calls = algo.getFunctionCallNumber()
print("function calls=", calls)
assert 0 < calls <= budget

# the design grows by exactly the number of true model evaluations
assert algo.getInputDoE().getSize() == initialSize + calls
assert algo.getOutputDoE().getSize() == initialSize + calls
ott.assert_almost_equal(algo.getOutputDoE(), model(algo.getInputDoE()))

result = algo.getResult()
assert result.getFunctionCallNumber() == calls
assert result.getHasConverged() == algo.getHasConverged()

# one history entry per loop iteration, plus the final estimate when the
# budget is exhausted: the U criterion adds one point per iteration
assert len(result.getProbabilityHistory()) == calls + 1
assert len(result.getReliabilityIndexHistory()) == calls + 1

# surrogate-based failure probability is close to the analytic value
probability = result.getProbabilityEstimate()
print("probability=", probability, "reference=", reference)
assert 0.0 < probability < 0.3
ott.assert_almost_equal(probability, reference, 0.05, 0.002)

# reliability index consistent with the probability estimate: Phi(-beta) = p
ott.assert_almost_equal(
    ot.Normal().computeCDF(-result.getReliabilityIndex()),
    probability,
    1.0e-12,
    1.0e-12,
)

# confidence intervals are ordered and bracket the estimate
probabilityCI = result.getProbabilityConfidenceInterval()
assert probabilityCI.getLowerBound()[0] <= probability <= probabilityCI.getUpperBound()[0]
indexCI = result.getReliabilityIndexConfidenceInterval()
assert indexCI.getLowerBound()[0] <= result.getReliabilityIndex() <= indexCI.getUpperBound()[0]

# the criterion and surrogate specification are exposed
assert algo.getCriterion().getReliabilityThreshold() == threshold
assert algo.getCovarianceModel().getInputDimension() == 2

# the wrapped simulator keeps its dynamic type through the handle
assert algo.getSimulationAlgorithm().getImplementation().getClassName() == "ProbabilitySimulationAlgorithm"

# a second run restarts from the initial design and reproduces the first one
algo.run()
assert algo.getFunctionCallNumber() == calls
assert algo.getInputDoE().getSize() == initialSize + calls
assert len(algo.getResult().getProbabilityHistory()) == calls + 1
ott.assert_almost_equal(algo.getResult().getProbabilityEstimate(), probability)

# persistence round-trip through a Study
if HAVE_LIBXML2:
    fileName = "t_ActiveLearningReliabilityAlgorithm_std.xml"
    study = ot.Study(fileName)
    study.add("algo", algo)
    study.save()
    study2 = ot.Study(fileName)
    study2.load()
    loaded = otexp.ActiveLearningReliabilityAlgorithm()
    study2.fillObject("algo", loaded)
    assert loaded.getFunctionCallNumber() == calls
    assert loaded.getInputDoE().getSize() == initialSize + calls
    assert loaded.getSimulationAlgorithm().getImplementation().getClassName() == "ProbabilitySimulationAlgorithm"
    ott.assert_almost_equal(loaded.getResult().getProbabilityEstimate(), probability)
    os.remove(fileName)

# same run with the EFF criterion also enriches the design
ot.RandomGenerator.SetSeed(0)
criterionEFF = otexp.ActiveLearningEFFFunction(threshold, 0.01)
assert criterionEFF.isMaximization()
algoEFF = otexp.ActiveLearningReliabilityAlgorithm(fitter, simulation, criterionEFF)
algoEFF.setMaximumIterations(budget)
algoEFF.setConvergenceCriterion(algoEFF.ACTIVE_LEARNING)
algoEFF.setSimulationAlgorithmSeed(0)
algoEFF.run()
callsEFF = algoEFF.getFunctionCallNumber()
print("EFF function calls=", callsEFF)
assert 0 < callsEFF <= budget
probabilityEFF = algoEFF.getResult().getProbabilityEstimate()
print("EFF probability=", probabilityEFF, "reference=", reference)
assert 0.0 < probabilityEFF < 0.3
ott.assert_almost_equal(probabilityEFF, reference, 0.05, 0.002)

# the enriched design is stored in the result surrogate
finalGPR = result.getGprResult()
assert finalGPR.getInputSample().getSize() == initialSize + calls

# per-iteration history: generic level, one entry per loop iteration
history = result.getSimulationResults()
assert len(history) == calls + 1
assert history[0].getImplementation().getClassName() == "ProbabilitySimulationResultImplementation"
for i in range(len(history)):
    ott.assert_almost_equal(
        history[i].getProbabilityEstimate(), result.getProbabilityHistory()[i]
    )
    assert history[i].getOuterSampling() > 0

# the algorithm exposes the same history directly
assert len(algo.getSimulationResults()) == calls + 1
for i in range(len(history)):
    ott.assert_almost_equal(
        algo.getSimulationResults()[i].getProbabilityEstimate(),
        result.getProbabilityHistory()[i],
    )

# history survives the Study round-trip
if HAVE_LIBXML2:
    loadedHistory = loaded.getResult().getSimulationResults()
    assert len(loadedHistory) == calls + 1
    ott.assert_almost_equal(
        loadedHistory[0].getProbabilityEstimate(), history[0].getProbabilityEstimate()
    )

# the four non-active-learning convergence modes all run, enrich the DoE
# and report an estimate of the correct order of magnitude
for mode, modeThreshold in [
    (algo.PROBABILITY_UNCERTAINTY, 0.1),
    (algo.RELIABILITY_INDEX_UNCERTAINTY, 0.1),
    (algo.PROBABILITY_STABILITY, 0.02),
    (algo.RELIABILITY_INDEX_STABILITY, 0.02),
]:
    ot.RandomGenerator.SetSeed(0)
    algoMode = otexp.ActiveLearningReliabilityAlgorithm(fitter, simulation, criterion)
    algoMode.setMaximumIterations(budget)
    algoMode.setConvergenceCriterion(mode)
    algoMode.setConvergenceCriterionThreshold(modeThreshold)
    algoMode.setSimulationAlgorithmSeed(0)
    algoMode.run()
    callsMode = algoMode.getFunctionCallNumber()
    probabilityMode = algoMode.getResult().getProbabilityEstimate()
    print(
        "mode=", mode,
        "calls=", callsMode,
        "probability=", probabilityMode,
        "converged=", algoMode.getHasConverged(),
    )
    assert 0 <= callsMode <= budget
    assert 0.0 < probabilityMode < 0.3
    assert len(algoMode.getResult().getProbabilityHistory()) == callsMode + 1
    ott.assert_almost_equal(probabilityMode, reference, 0.05, 0.002)
    # a stable criterion must really satisfy its own convergence test
    if mode in (algo.PROBABILITY_STABILITY, algo.RELIABILITY_INDEX_STABILITY) and algoMode.getHasConverged():
        modeHistory = (
            algoMode.getResult().getProbabilityHistory()
            if mode == algo.PROBABILITY_STABILITY
            else algoMode.getResult().getReliabilityIndexHistory()
        )
        relativeVariation = abs(modeHistory[-1] - modeHistory[-2]) / abs(modeHistory[-1])
        assert relativeVariation <= modeThreshold + 1.0e-12

# setEvent propagates to the wrapped simulator and to the run itself
ot.RandomGenerator.SetSeed(0)
newThreshold = 1.0
newReference = 1.0 - ot.Normal().computeCDF(newThreshold / 5.0**0.5)
algoEvent = otexp.ActiveLearningReliabilityAlgorithm(fitter, simulation, criterion)
algoEvent.setMaximumIterations(budget)
algoEvent.setSimulationAlgorithmSeed(0)
algoEvent.setEvent(ot.ThresholdEvent(Y, ot.Greater(), newThreshold))
assert algoEvent.getEvent().getThreshold() == newThreshold
assert algoEvent.getSimulationAlgorithm().getEvent().getThreshold() == newThreshold
# the criterion follows the new limit state, so enrichment is selected against it
assert algoEvent.getCriterion().getReliabilityThreshold() == newThreshold
algoEvent.run()
print("new event probability=", algoEvent.getResult().getProbabilityEstimate(), "reference=", newReference)
ott.assert_almost_equal(algoEvent.getResult().getProbabilityEstimate(), newReference, 0.05, 0.002)

# a default-constructed algorithm is persistable and its criterion is usable
defaultAlgo = otexp.ActiveLearningReliabilityAlgorithm()
assert defaultAlgo.getMaximumIterations() >= 1
assert defaultAlgo.getCriterion().getImplementation().getClassName() == "ActiveLearningReliabilityFunctionImplementation"
if HAVE_LIBXML2:
    fileNameDefault = "t_ActiveLearningReliabilityAlgorithm_default.xml"
    studyDefault = ot.Study(fileNameDefault)
    studyDefault.add("algo", defaultAlgo)
    studyDefault.save()
    studyDefault2 = ot.Study(fileNameDefault)
    studyDefault2.load()
    loadedDefault = otexp.ActiveLearningReliabilityAlgorithm()
    studyDefault2.fillObject("algo", loadedDefault)
    assert loadedDefault.getMaximumIterations() == defaultAlgo.getMaximumIterations()
    assert loadedDefault.getCriterion().getImplementation().getClassName() == "ActiveLearningReliabilityFunctionImplementation"
    os.remove(fileNameDefault)

# full detail on a NAIS run: weights through the downcast, before and after save
ot.RandomGenerator.SetSeed(0)
simNAIS = ot.NAIS(event)
simNAIS.setMaximumOuterSampling(200)
simNAIS.setBlockSize(50)
algoNAIS = otexp.ActiveLearningReliabilityAlgorithm(fitter, simNAIS, criterion)
algoNAIS.setMaximumIterations(1)
algoNAIS.setSimulationAlgorithmSeed(0)
algoNAIS.run()
histNAIS = algoNAIS.getResult().getSimulationResults()
assert len(histNAIS) == len(algoNAIS.getResult().getProbabilityHistory())
assert len(histNAIS) >= 1
naisImpl = histNAIS[0].getImplementation()
assert naisImpl.getClassName() == "NAISResult"
assert len(naisImpl.getWeights()) > 0
ott.assert_almost_equal(
    histNAIS[0].getProbabilityEstimate(), naisImpl.getProbabilityEstimate()
)

if HAVE_LIBXML2:
    fileNameNAIS = "t_ActiveLearningReliabilityAlgorithm_nais.xml"
    studyN = ot.Study(fileNameNAIS)
    studyN.add("algo", algoNAIS)
    studyN.save()
    studyN2 = ot.Study(fileNameNAIS)
    studyN2.load()
    loadedNAIS = otexp.ActiveLearningReliabilityAlgorithm()
    studyN2.fillObject("algo", loadedNAIS)
    loadedNaisImpl = loadedNAIS.getResult().getSimulationResults()[0].getImplementation()
    assert loadedNaisImpl.getClassName() == "NAISResult"
    assert len(loadedNaisImpl.getWeights()) > 0
    ott.assert_almost_equal(
        loadedNaisImpl.getProbabilityEstimate(), naisImpl.getProbabilityEstimate()
    )
    ott.assert_almost_equal(
        loadedNaisImpl.getAuxiliaryOutputSample(), naisImpl.getAuxiliaryOutputSample()
    )
    os.remove(fileNameNAIS)

# SubsetSampling run: base-complete history with concrete coefficient of variation
ot.RandomGenerator.SetSeed(0)
simSubset = ot.SubsetSampling(event)
algoSubset = otexp.ActiveLearningReliabilityAlgorithm(fitter, simSubset, criterion)
algoSubset.setMaximumIterations(1)
algoSubset.setSimulationAlgorithmSeed(0)
algoSubset.run()
histSubset = algoSubset.getResult().getSimulationResults()
assert len(histSubset) == len(algoSubset.getResult().getProbabilityHistory())
assert len(histSubset) >= 1
subsetImpl = histSubset[0].getImplementation()
assert subsetImpl.getClassName() == "SubsetSamplingResult"
ott.assert_almost_equal(
    subsetImpl.getProbabilityEstimate(), histSubset[0].getProbabilityEstimate()
)
assert subsetImpl.getCoefficientOfVariation() >= 0.0
