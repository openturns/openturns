#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
# ot.Log.Show(ot.Log.ALL)

dim = 2

# problem
model = ot.SymbolicFunction(
    ["x", "y"],
    ["3*(1-x)^2*exp(-x^2-(y+1)^2)-10*(x/5-x^3-y^5)*exp(-x^2-y^2)-exp(-(x+1)^2-y^2)/3"],
)
bounds = ot.Interval([-3.0] * dim, [3.0] * dim)
problem = ot.OptimizationProblem(model)
problem.setBounds(bounds)

# solver
solver = ot.TNC(problem)

# run locally
solver.setStartingPoint([0.0] * dim)
algo = solver
algo.run()
result = algo.getResult()
local_optimal_point = [0.296446, 0.320196]
local_optimal_value = [-0.0649359]
ott.assert_almost_equal(result.getOptimalPoint(), local_optimal_point, 1e-5, 0.0)
ott.assert_almost_equal(result.getOptimalValue(), local_optimal_value, 1e-5, 0.0)


# multistart
lower_bound = bounds.getLowerBound()
upper_bound = bounds.getUpperBound()
dist_collection = []
for i in range(dim):
    dist_collection.append(ot.Uniform(lower_bound[i], upper_bound[i]))
distribution = ot.JointDistribution(dist_collection)
size = 20
experiment = ot.LHSExperiment(distribution, size)
startingSample = experiment.generate()
algo = ot.MultiStart(solver, startingSample)
algo.setMaximumCallsNumber(2000)
algo.run()
result = algo.getResult()
true_optimal_point = [0.228279, -1.62553]
true_optimal_value = [-6.55113]
ott.assert_almost_equal(result.getOptimalPoint(), true_optimal_point, 1e-5, 0.0)
ott.assert_almost_equal(result.getOptimalValue(), true_optimal_value, 1e-5, 0.0)
n_local_results = algo.getResultCollection().getSize()
assert n_local_results == 20, "n_local_results is wrong"
algo.setKeepResults(False)
algo.run()
n_local_results_nokeep = algo.getResultCollection().getSize()
assert n_local_results_nokeep == 0, "n_local_results_nokeep is wrong"
assert result.getCallsNumber() < 400, "wrong calls number"

# inner progress is rescaled to the global budget and stays monotonic
progress_calls = []


def progress(p):
    progress_calls.append(p)


def no_stop():
    return False


def stop_now():
    return True


ot.RandomGenerator.SetSeed(0)
small_distribution = ot.JointDistribution([ot.Uniform(-3.0, 3.0)] * dim)
small_sample = small_distribution.getSample(5)
cb_algo = ot.MultiStart(ot.TNC(problem), small_sample)
cb_algo.setMaximumCallsNumber(2000)
cb_algo.setProgressCallback(progress)
cb_algo.setStopCallback(no_stop)
cb_algo.run()
assert cb_algo.getResult().getStatus() == ot.OptimizationResult.SUCCESS
assert len(progress_calls) > 0, "progress callback was not invoked"
assert all(0.0 <= p <= 100.0 for p in progress_calls), "progress out of range"
assert all(b >= a for a, b in zip(progress_calls, progress_calls[1:])), "progress not monotonic"

# user stop interrupts the current local search, not just between restarts
inner = ot.TNC(problem)
inner.setCheckStatus(False)
stop_algo = ot.MultiStart(inner, small_sample)
stop_algo.setMaximumCallsNumber(2000)
stop_algo.setCheckStatus(False)
stop_algo.setKeepResults(True)
stop_algo.setStopCallback(stop_now)
stop_algo.run()
assert stop_algo.getResult().getStatus() == ot.OptimizationResult.INTERRUPTION
assert stop_algo.getResultCollection().getSize() == 1, "wrong restarts number"
assert stop_algo.getResult().getCallsNumber() < 2000, "stop was not honored"
