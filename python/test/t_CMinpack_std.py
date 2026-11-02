#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import sys
import os

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(3)
ot.Log.Show(ot.Log.ALL)


def progress(percent):
    sys.stderr.write("-- progress=" + str(percent) + "%\n")


def stop():
    sys.stderr.write("-- stop?\n")
    return False


n = 3
m = 10

x = [[0.5 + i] for i in range(m)]


model = ot.SymbolicFunction(["a", "b", "c", "x"], ["a + b * exp(c * x)"])
p_ref = [2.8, 1.2, 0.5]  # a, b, c
modelx = ot.ParametricFunction(model, [0, 1, 2], p_ref)
y = modelx(x)


def residualFunction_py(p):
    modelx = ot.ParametricFunction(model, [0, 1, 2], p)
    return [modelx(x[i])[0] - y[i, 0] for i in range(m)]


residualFunction = ot.PythonFunction(n, m, residualFunction_py)

bounds = ot.Interval([0, 0, 0], [2.5, 8.0, 19])


for bound in [True, False]:
    problem = ot.LeastSquaresProblem(residualFunction)
    if bound:
        problem.setBounds(bounds)
    startingPoint = [1.0] * n
    algo = ot.CMinpack(problem)
    algo.setStartingPoint(startingPoint)
    # algo.setProgressCallback(progress)
    # algo.setStopCallback(stop)
    algo.run()
    result = algo.getResult()
    # print(result.getInputSample())
    # print(result.getOutputSample())
    x_star = result.getOptimalPoint()
    print(result)
    if bound:
        assert x_star in bounds, "optimal point not in bounds"
    else:
        ott.assert_almost_equal(x_star, p_ref)

# CMinpack error paths and accessors (deterministic)
print("repr=", repr(algo))
assert "CMinpack" in repr(algo)
assert "CMinpack" in str(algo)

# only least-squares problems are supported
with ott.assert_raises(Exception):
    ot.CMinpack(ot.OptimizationProblem(ot.SymbolicFunction(["x"], ["x^2"])))

# underdetermined problems are rejected
with ott.assert_raises(Exception):
    ot.CMinpack(
        ot.LeastSquaresProblem(ot.SymbolicFunction(["x1", "x2", "x3"], ["x1+x2+x3"]))
    )

# one-sided bounds are rejected
oneSided = ot.Interval([0.0] * n, [1.0] * n)
oneSided.setFiniteUpperBound([True] * (n - 1) + [False])
oneSidedProblem = ot.LeastSquaresProblem(residualFunction)
oneSidedProblem.setBounds(oneSided)
with ott.assert_raises(Exception):
    ot.CMinpack(oneSidedProblem)

# constrained problems are rejected
ineqProblem = ot.LeastSquaresProblem(residualFunction)
ineqProblem.setInequalityConstraint(ot.SymbolicFunction(["a", "b", "c"], ["a"]))
with ott.assert_raises(Exception):
    ot.CMinpack(ineqProblem)
eqProblem = ot.LeastSquaresProblem(residualFunction)
eqProblem.setEqualityConstraint(ot.SymbolicFunction(["a", "b", "c"], ["a"]))
with ott.assert_raises(Exception):
    ot.CMinpack(eqProblem)

# invalid starting point dimension
badStartAlgo = ot.CMinpack(ot.LeastSquaresProblem(residualFunction))
badStartAlgo.setStartingPoint([1.0] * (n + 1))
with ott.assert_raises(Exception):
    badStartAlgo.run()

# starting point outside bounds
outProblem = ot.LeastSquaresProblem(residualFunction)
outProblem.setBounds(bounds)
outAlgo = ot.CMinpack(outProblem)
outAlgo.setStartingPoint([10.0] * n)
with ott.assert_raises(Exception):
    outAlgo.run()

# progress/stop callbacks and user stop
unboundProblem = ot.LeastSquaresProblem(residualFunction)
cbAlgo = ot.CMinpack(unboundProblem)
cbAlgo.setStartingPoint([1.0] * n)
cbAlgo.setProgressCallback(progress)
cbAlgo.setStopCallback(stop)
cbAlgo.run()
ott.assert_almost_equal(cbAlgo.getResult().getOptimalPoint(), p_ref)


def stopNow():
    return True


stopAlgo = ot.CMinpack(ot.LeastSquaresProblem(residualFunction))
stopAlgo.setStartingPoint([1.0] * n)
stopAlgo.setMaximumCallsNumber(10000)
stopAlgo.setStopCallback(stopNow)
stopAlgo.run()
assert stopAlgo.getResult().getStatusMessage() == "user stop"

# persistence
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("cminpack_std.xml"))
study.add("algo", cbAlgo)
study.save()

os.remove("cminpack_std.xml")
