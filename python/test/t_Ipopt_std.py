#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott
import sys
import os


def progress(percent):
    sys.stderr.write("-- progress=" + str(percent) + "%\n")


def stop():
    sys.stderr.write("-- stop?\n")
    return False


# Definition of objective function
f = ot.SymbolicFunction(["x1", "x2"], ["-(x2 - 2.0) * (x2 - 2.0)"])
bounds = ot.Interval([-1.0, -1.0e19], [1.0, 1.0e19])
g = ot.SymbolicFunction(["x1", "x2"], ["-(x1 * x1 + x2 - 1.0)"])

# Setting up problem
problem = ot.OptimizationProblem(f)
problem.setBounds(bounds)
problem.setEqualityConstraint(g)

algo = ot.Ipopt(problem)
algo.setStartingPoint([0.5, 1.5])
algo.setMaximumCallsNumber(10000)
algo.setProgressCallback(progress)
algo.setStopCallback(stop)

# ot.ResourceMap.AddAsScalar('Ipopt-max_cpu_time', 15.0)

algo.run()
result = algo.getResult()
print(" -- Optimal point = " + result.getOptimalPoint().__str__())
print(" -- Optimal value = " + result.getOptimalValue().__str__())
print(" -- Evaluation number = " + result.getInputSample().getSize().__str__())
ott.assert_almost_equal(result.getOptimalPoint(), [1.0, 0.0], 1e-5, 1e-4)

# extra coverage: accessors, repr/str, save/load, error branches
assert isinstance(repr(algo), str)
assert isinstance(str(algo), str)
defaultAlgo = ot.Ipopt()
assert isinstance(repr(defaultAlgo), str)
assert isinstance(str(defaultAlgo), str)
with ott.assert_raises(Exception):
    ot.Ipopt(ot.OptimizationProblem(ot.SymbolicFunction(["x"], ["x", "2*x"])))
with ott.assert_raises(Exception):
    ot.Ipopt(ot.LeastSquaresProblem(ot.SymbolicFunction(["x"], ["x^2"])))
nonContinuous = ot.OptimizationProblem(f)
nonContinuous.setVariablesType([ot.OptimizationProblemImplementation.INTEGER] * 2)
with ott.assert_raises(Exception):
    ot.Ipopt(nonContinuous)
badStart = ot.Ipopt(problem)
badStart.setStartingPoint([0.0])
with ott.assert_raises(Exception):
    badStart.run()
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("ipopt.xml"))
study.add("algo", algo)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("ipopt.xml"))
loadedStudy.load()
assert loadedStudy.hasObject("algo")

os.remove("ipopt.xml")
