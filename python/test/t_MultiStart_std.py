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
distribution = ot.ComposedDistribution(dist_collection)
size = 20
experiment = ot.LHSExperiment(distribution, size)
startingSample = experiment.generate()
algo = ot.MultiStart(solver, startingSample)
algo.setMaximumEvaluationNumber(2000)
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
