#! /usr/bin/env python

import os

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# --- SimulationResultImplementation tests ---

# Default constructor
impl = ot.SimulationResultImplementation()
assert impl.getOuterSampling() == 0
assert impl.getBlockSize() == 0
assert impl.getTimeDuration() == 0.0

# Parameterized constructor
impl2 = ot.SimulationResultImplementation(100, 16)
assert impl2.getOuterSampling() == 100
assert impl2.getBlockSize() == 16
assert impl2.getTimeDuration() == 0.0

# Setters
impl2.setOuterSampling(200)
assert impl2.getOuterSampling() == 200

impl2.setBlockSize(32)
assert impl2.getBlockSize() == 32

impl2.setTimeDuration(1.5)
ott.assert_almost_equal(impl2.getTimeDuration(), 1.5)

# repr
repr_str = repr(impl2)
assert "outerSampling=200" in repr_str
assert "blockSize=32" in repr_str

# Equality / inequality
impl3 = ot.SimulationResultImplementation(200, 32)
impl3.setTimeDuration(1.5)
assert impl2 == impl3
assert not (impl2 != impl3)

impl4 = ot.SimulationResultImplementation(200, 32)
impl4.setTimeDuration(2.0)
assert impl2 != impl4
assert not (impl2 == impl4)

impl5 = ot.SimulationResultImplementation(999, 32)
impl5.setTimeDuration(1.5)
assert impl2 != impl5

# Equality with non-SimulationResultImplementation returns False
assert impl2 != "not a result"

# Copy through handle preserves implementation
result_copy = ot.SimulationResult(impl2)
result_copy.setOuterSampling(500)
assert impl2.getOuterSampling() == 200

# --- Serialization tests ---

# Save and load via XML
impl_file = "t_SimulationResult_impl.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(impl_file))
study.add("impl", impl2)
study.save()
study2 = ot.Study()
study2.setStorageManager(ot.XMLStorageManager(impl_file))
study2.load()
loaded_impl = ot.SimulationResultImplementation()
study2.fillObject("impl", loaded_impl)
assert loaded_impl == impl2
os.remove(impl_file)

# --- SimulationResult handle class tests ---

# Default constructor
result = ot.SimulationResult()
assert result.getOuterSampling() == 0
assert result.getBlockSize() == 0
assert result.getTimeDuration() == 0.0

# Constructor from implementation
result2 = ot.SimulationResult(impl2)
assert result2.getOuterSampling() == 200
assert result2.getBlockSize() == 32
ott.assert_almost_equal(result2.getTimeDuration(), 1.5)

# Setters with copy-on-write
result3 = ot.SimulationResult(impl2)
result3.setOuterSampling(50)
assert result3.getOuterSampling() == 50
assert result2.getOuterSampling() == 200

result3.setBlockSize(8)
assert result3.getBlockSize() == 8
assert result2.getBlockSize() == 32

result3.setTimeDuration(3.0)
ott.assert_almost_equal(result3.getTimeDuration(), 3.0)
ott.assert_almost_equal(result2.getTimeDuration(), 1.5)

# repr
repr_str2 = repr(result2)
assert "outerSampling=200" in repr_str2
assert "blockSize=32" in repr_str2

# Equality / inequality for handle
result4 = ot.SimulationResult(impl2)
assert result2 == result4
assert not (result2 != result4)

result5 = ot.SimulationResult(ot.SimulationResultImplementation(100, 10))
assert result2 != result5
assert not (result2 == result5)

# --- Tests via ProbabilitySimulationResult subclass ---

model = ot.SymbolicFunction(["x0", "x1"], ["x0 + x1"])
dist = ot.Normal(2)
vect = ot.RandomVector(dist)
output = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(output, ot.Greater(), 1.0)

ot.RandomGenerator.SetSeed(0)
algo = ot.ProbabilitySimulationAlgorithm(event, ot.MonteCarloExperiment())
algo.setMaximumOuterSampling(100)
algo.setBlockSize(10)
algo.setMaximumCoefficientOfVariation(0.0)
algo.setMaximumStandardDeviation(0.1)
algo.run()

prob_result = ot.ProbabilitySimulationResult(algo.getResult())

# Verify base class accessors work on the subclass
assert prob_result.getOuterSampling() > 0
assert prob_result.getBlockSize() == 10
assert prob_result.getTimeDuration() >= 0.0

# ProbabilitySimulationResult specific
assert prob_result.getProbabilityEstimate() > 0
assert prob_result.getVarianceEstimate() >= 0
assert prob_result.getCoefficientOfVariation() >= 0
assert prob_result.getStandardDeviation() >= 0
dist_prob = prob_result.getProbabilityDistribution()
assert dist_prob.getDimension() == 1

# ProbabilitySimulationResult equality
prob_result2 = ot.ProbabilitySimulationResult(algo.getResult())
assert prob_result == prob_result2
assert not (prob_result != prob_result2)

# Different probability estimate
diff_result = ot.ProbabilitySimulationResult(event, 0.99, 0.01, 10, 5)
assert prob_result != diff_result

# ProbabilitySimulationResult serialization
prob_file = "t_SimulationResult_prob.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(prob_file))
study.add("prob_result", prob_result)
study.save()
study2 = ot.Study()
study2.setStorageManager(ot.XMLStorageManager(prob_file))
study2.load()
loaded_prob = ot.ProbabilitySimulationResult()
study2.fillObject("prob_result", loaded_prob)
assert loaded_prob.getOuterSampling() == prob_result.getOuterSampling()
assert loaded_prob.getBlockSize() == prob_result.getBlockSize()
ott.assert_almost_equal(
    loaded_prob.getProbabilityEstimate(), prob_result.getProbabilityEstimate()
)
ott.assert_almost_equal(
    loaded_prob.getVarianceEstimate(), prob_result.getVarianceEstimate()
)
os.remove(prob_file)

# --- ExpectationSimulationResult subclass test ---

model2 = ot.SymbolicFunction(["x0", "x1"], ["x0 + x1", "x0 * x1"])
dist2 = ot.Normal(2)
vect2 = ot.RandomVector(dist2)
output2 = ot.CompositeRandomVector(model2, vect2)

ot.RandomGenerator.SetSeed(0)
algo2 = ot.ExpectationSimulationAlgorithm(output2)
algo2.setMaximumOuterSampling(500)
algo2.setBlockSize(2)
algo2.run()

expect_result = ot.ExpectationSimulationResult(algo2.getResult())
assert expect_result.getOuterSampling() > 0
assert expect_result.getBlockSize() == 2
ott.assert_almost_equal(
    expect_result.getExpectationEstimate(),
    output2.getSample(100000).computeMean(),
    0.2,
    0.1,
)
expect_dist = expect_result.getExpectationDistribution()
assert expect_dist.getDimension() == 2

# ExpectationSimulationResult equality
expect_result2 = ot.ExpectationSimulationResult(algo2.getResult())
assert expect_result == expect_result2

# --- SobolSimulationResult subclass test ---

model3 = ot.SymbolicFunction(["x0", "x1"], ["x0 + 2 * x1"])
dist3 = ot.JointDistribution([ot.Uniform(0, 1), ot.Uniform(0, 1)])

estimator = ot.SaltelliSensitivityAlgorithm()
ot.RandomGenerator.SetSeed(0)
algo3 = ot.SobolSimulationAlgorithm(dist3, model3, estimator)
algo3.setMaximumOuterSampling(5)
algo3.setExperimentSize(100)
algo3.setBlockSize(1)
algo3.setMaximumCoefficientOfVariation(-1.0)
algo3.run()

sobol_result = ot.SobolSimulationResult(algo3.getResult())
assert sobol_result.getOuterSampling() > 0
assert sobol_result.getBlockSize() == 1
first_idx = sobol_result.getFirstOrderIndicesEstimate()
total_idx = sobol_result.getTotalOrderIndicesEstimate()
assert first_idx.getDimension() == 2
assert total_idx.getDimension() == 2
assert sobol_result.getExperimentSize() > 0

# Sobol indices distribution access
sobol_result2 = ot.SobolSimulationResult(algo3.getResult())
assert sobol_result == sobol_result2

print("All SimulationResult tests passed.")
