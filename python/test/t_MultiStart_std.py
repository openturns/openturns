#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
# ot.Log.Show(ot.Log.ALL)

dim = 2

# problem
model = ot.Function(['x', 'y'], ['z'],
                    ['3*(1-x)^2*exp(-x^2-(y+1)^2)-10*(x/5-x^3-y^5)*exp(-x^2-y^2)-exp(-(x+1)^2-y^2)/3'])
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
print('-- local search x*=', result.getOptimalPoint(),
      'f(x*)=', result.getOptimalValue())

# multistart
distribution = ot.Normal([0.0] * dim, [2.0] * dim, ot.CorrelationMatrix(dim))
experiment = ot.LHSExperiment(distribution, 20)
startingPoints = experiment.generate()
algo = ot.MultiStart(solver, startingPoints)
print('-- algo=', algo)
algo.run()
result = algo.getResult()
print('-- multistart x*=', result.getOptimalPoint(),
      'f(x*)=', result.getOptimalValue())
