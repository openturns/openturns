#! /usr/bin/env python

import os

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# --- SimulationAlgorithmImplementation tests ---

# Default constructor
impl = ot.SimulationAlgorithmImplementation()
assert impl.getBlockSize() >= 1
assert impl.getMaximumOuterSampling() >= 0
assert impl.getMaximumCoefficientOfVariation() >= 0.0
assert impl.getMaximumStandardDeviation() >= 0.0
assert impl.getMaximumTimeDuration() >= -1.0

# Setters
impl.setMaximumOuterSampling(500)
assert impl.getMaximumOuterSampling() == 500

impl.setMaximumCoefficientOfVariation(0.01)
ott.assert_almost_equal(impl.getMaximumCoefficientOfVariation(), 0.01)

impl.setMaximumStandardDeviation(0.1)
ott.assert_almost_equal(impl.getMaximumStandardDeviation(), 0.1)

impl.setMaximumTimeDuration(60.0)
ott.assert_almost_equal(impl.getMaximumTimeDuration(), 60.0)

impl.setBlockSize(16)
assert impl.getBlockSize() == 16

# Convergence strategy
strategy = ot.Compact()
impl.setConvergenceStrategy(strategy)
assert impl.getConvergenceStrategy().getSample().getSize() == 0

# repr
repr_str = repr(impl)
assert "SimulationAlgorithmImplementation" in repr_str
assert "maximumOuterSampling=500" in repr_str
assert "maximumCoefficientOfVariation=0.01" in repr_str
assert "maximumStandardDeviation=0.1" in repr_str
assert "blockSize=16" in repr_str

# Equality / inequality
impl2 = ot.SimulationAlgorithmImplementation()
impl2.setMaximumOuterSampling(500)
impl2.setMaximumCoefficientOfVariation(0.01)
impl2.setMaximumStandardDeviation(0.1)
impl2.setMaximumTimeDuration(60.0)
impl2.setBlockSize(16)
impl2.setConvergenceStrategy(strategy)
assert impl == impl2
assert not (impl != impl2)

impl3 = ot.SimulationAlgorithmImplementation()
impl3.setMaximumOuterSampling(999)
assert impl != impl3
assert not (impl == impl3)

# Equality with non-SimulationAlgorithmImplementation returns False
assert impl != "not an algorithm"

# Clone (copy constructor)
impl_clone = ot.SimulationAlgorithmImplementation(impl)
assert impl == impl_clone
impl_clone.setMaximumOuterSampling(100)
assert impl.getMaximumOuterSampling() == 500

# Virtual run() throws NotYetImplementedException
with ott.assert_raises(RuntimeError):
    impl.run()

# Block size validation
with ott.assert_raises(TypeError):
    impl.setBlockSize(0)

# --- Serialization tests ---

impl_file = "t_SimulationAlgorithm_impl.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(impl_file))
study.add("impl", impl)
study.save()
study2 = ot.Study()
study2.setStorageManager(ot.XMLStorageManager(impl_file))
study2.load()
loaded_impl = ot.SimulationAlgorithmImplementation()
study2.fillObject("impl", loaded_impl)
assert loaded_impl == impl
os.remove(impl_file)

# --- SimulationAlgorithm handle class tests ---

# Default constructor
algo = ot.SimulationAlgorithm()
assert algo.getBlockSize() >= 1
assert algo.getMaximumOuterSampling() >= 0

# Constructor from implementation
algo2 = ot.SimulationAlgorithm(impl)
assert algo2.getMaximumOuterSampling() == 500
assert algo2.getBlockSize() == 16
ott.assert_almost_equal(algo2.getMaximumCoefficientOfVariation(), 0.01)
ott.assert_almost_equal(algo2.getMaximumStandardDeviation(), 0.1)
ott.assert_almost_equal(algo2.getMaximumTimeDuration(), 60.0)

# Setters with copy-on-write
algo3 = ot.SimulationAlgorithm(impl)
algo3.setMaximumOuterSampling(200)
assert algo3.getMaximumOuterSampling() == 200
assert impl.getMaximumOuterSampling() == 500

algo3.setBlockSize(8)
assert algo3.getBlockSize() == 8
assert impl.getBlockSize() == 16

algo3.setMaximumCoefficientOfVariation(0.5)
ott.assert_almost_equal(algo3.getMaximumCoefficientOfVariation(), 0.5)
ott.assert_almost_equal(impl.getMaximumCoefficientOfVariation(), 0.01)

algo3.setMaximumStandardDeviation(0.9)
ott.assert_almost_equal(algo3.getMaximumStandardDeviation(), 0.9)
ott.assert_almost_equal(impl.getMaximumStandardDeviation(), 0.1)

algo3.setMaximumTimeDuration(120.0)
ott.assert_almost_equal(algo3.getMaximumTimeDuration(), 120.0)
ott.assert_almost_equal(impl.getMaximumTimeDuration(), 60.0)

# Convergence strategy via handle
strategy2 = ot.Last()
algo3.setConvergenceStrategy(strategy2)
assert algo3.getConvergenceStrategy().getSample().getSize() == 0
assert impl.getConvergenceStrategy().getSample().getSize() == 0

# repr
repr_str2 = repr(algo2)
assert "SimulationAlgorithmImplementation" in repr_str2
assert "maximumOuterSampling=500" in repr_str2

# Equality / inequality for handle
algo4 = ot.SimulationAlgorithm(impl)
assert algo2 == algo4
assert not (algo2 != algo4)

algo5 = ot.SimulationAlgorithm(ot.SimulationAlgorithmImplementation())
assert algo2 != algo5
assert not (algo2 == algo5)

# --- Progress/stop callbacks ---

progress_called = [False]
stop_called = [False]


def my_progress(percent):
    progress_called[0] = True


def my_stop():
    stop_called[0] = True
    return True


algo2.setProgressCallback(my_progress)
algo2.setStopCallback(my_stop)

# Verify callbacks don't crash on the handle
# (concrete run will be tested below)

# --- Serialization via handle ---

algo_file = "t_SimulationAlgorithm_algo.xml"
study3 = ot.Study()
study3.setStorageManager(ot.XMLStorageManager(algo_file))
study3.add("algo", algo2)
study3.save()
study4 = ot.Study()
study4.setStorageManager(ot.XMLStorageManager(algo_file))
study4.load()
loaded_algo = ot.SimulationAlgorithm()
study4.fillObject("algo", loaded_algo)
assert loaded_algo == algo2
os.remove(algo_file)

# --- Test through EventSimulation subclass ---

X = ot.RandomVector(ot.Normal())
Y = ot.CompositeRandomVector(ot.SymbolicFunction(["X"], ["X"]), X)
event = ot.ThresholdEvent(Y, ot.Less(), -2.0)

event_algo = ot.ProbabilitySimulationAlgorithm(event, ot.MonteCarloExperiment())
event_algo.setMaximumOuterSampling(100)
event_algo.setBlockSize(10)
event_algo.setMaximumCoefficientOfVariation(-1.0)
event_algo.setMaximumStandardDeviation(-1.0)
event_algo.run()

result = event_algo.getResult()
assert result.getOuterSampling() > 0
assert result.getBlockSize() == 10
assert result.getProbabilityEstimate() > 0
assert result.getTimeDuration() >= 0.0

# Verify handle accessors reflect implementation state
assert event_algo.getMaximumOuterSampling() == 100
assert event_algo.getBlockSize() == 10

# --- Test through ExpectationSimulationAlgorithm subclass ---

model = ot.SymbolicFunction(["X0", "X1"], ["X0 + X1"])
dist = ot.JointDistribution([ot.Normal(), ot.Normal()])
vect = ot.RandomVector(dist)
output = ot.CompositeRandomVector(model, vect)

ot.RandomGenerator.SetSeed(0)
expect_algo = ot.ExpectationSimulationAlgorithm(output)
expect_algo.setMaximumOuterSampling(500)
expect_algo.setBlockSize(4)
expect_algo.setMaximumCoefficientOfVariation(-1.0)
expect_algo.run()

expect_result = expect_algo.getResult()
assert expect_result.getOuterSampling() > 0
assert expect_result.getBlockSize() == 4
assert expect_result.getExpectationEstimate().getDimension() == 1
assert expect_result.getTimeDuration() >= 0.0

# Verify convergence strategy works
history = expect_algo.getConvergenceStrategy()
assert history.getSample().getSize() > 0

# --- Test through SobolSimulationAlgorithm subclass ---

model2 = ot.SymbolicFunction(["X0", "X1"], ["X0 + 2 * X1"])
dist2 = ot.JointDistribution([ot.Uniform(0, 1), ot.Uniform(0, 1)])
estimator = ot.SaltelliSensitivityAlgorithm()

ot.RandomGenerator.SetSeed(0)
sobol_algo = ot.SobolSimulationAlgorithm(dist2, model2, estimator)
sobol_algo.setMaximumOuterSampling(5)
sobol_algo.setExperimentSize(100)
sobol_algo.setBlockSize(1)
sobol_algo.setMaximumCoefficientOfVariation(-1.0)
sobol_algo.run()

sobol_result = sobol_algo.getResult()
assert sobol_result.getOuterSampling() > 0
assert sobol_result.getBlockSize() == 1
assert sobol_result.getFirstOrderIndicesEstimate().getDimension() == 2
assert sobol_result.getTotalOrderIndicesEstimate().getDimension() == 2
assert sobol_result.getTimeDuration() >= 0.0

print("All SimulationAlgorithm tests passed.")
