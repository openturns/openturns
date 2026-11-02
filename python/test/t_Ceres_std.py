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


algoNames = ot.Ceres.GetAlgorithmNames()
print(algoNames)

# unconstrained optimization: rosenbrock x*=(1,1), x*=(0.7864, 0.6177) on unit disk
dim = 2
f = ot.SymbolicFunction(["x1", "x2"], ["1+100*(x2-x1^2)^2+(1-x1)^2"])
startingPoint = [1e-3] * dim
p_ref = [1.0] * dim

for algoName in algoNames:
    if algoName in ["LEVENBERG_MARQUARDT", "DOGLEG"]:
        # only test general optimization algorithms
        continue
    for minimization in [True, False]:
        if algoName == "NONLINEAR_CONJUGATE_GRADIENT" and not minimization:
            # goes very far and the function cannot evaluate
            continue
        print("algoName=", algoName, "minimization=", minimization)
        problem = ot.OptimizationProblem(f)
        problem.setMinimization(minimization)
        algo = ot.Ceres(problem, algoName)
        algo.setMaximumIterationNumber(10000)
        algo.setStartingPoint(startingPoint)
        # algo.setProgressCallback(progress)
        # algo.setStopCallback(stop)
        algo.run()
        result = algo.getResult()
        x_star = result.getOptimalPoint()
        if minimization and algoName != "STEEPEST_DESCENT":
            ott.assert_almost_equal(x_star, p_ref, 5e-2)
        print(result)


# least-squares optimization
n = 3
m = 10

x = [[0.5 + i] for i in range(m)]


model = ot.SymbolicFunction(["a", "b", "c", "x"], ["a + b * exp(min(500, c * x))"])
p_ref = [2.8, 1.2, 0.5]  # a, b, c
modelx = ot.ParametricFunction(model, [0, 1, 2], p_ref)
y = modelx(x)


def residualFunction_py(p):
    modelx = ot.ParametricFunction(model, [0, 1, 2], p)
    return [modelx(x[i])[0] - y[i, 0] for i in range(m)]


residualFunction = ot.PythonFunction(n, m, residualFunction_py)

bounds = ot.Interval([0, 0, 0], [2.5, 8.0, 19])

for algoName in algoNames:
    line_search = not (algoName in ["LEVENBERG_MARQUARDT", "DOGLEG"])
    for bound in [True, False]:
        if bound and line_search:
            # line search do not support bound constraints
            continue
        print("algoName=", algoName, "bound=", bound)
        problem = ot.LeastSquaresProblem(residualFunction)
        if bound:
            problem.setBounds(bounds)
        startingPoint = [1.0] * n
        algo = ot.Ceres(problem, algoName)
        algo.setStartingPoint(startingPoint)
        # algo.setProgressCallback(progress)
        # algo.setStopCallback(stop)
        algo.setMaximumIterationNumber(10000)
        algo.run()
        result = algo.getResult()
        x_star = result.getOptimalPoint()
        print(result)
        if bound:
            assert x_star in bounds, "optimal point not in bounds"
        else:
            if not line_search:
                # line search algorithms converge less well
                ott.assert_almost_equal(x_star, p_ref, 0.1)

# extra coverage: repr/str, algorithm name, checkProblem branches, save/load
assert isinstance(repr(algo), str)
assert isinstance(str(algo), str)
with ott.assert_raises(Exception):
    ot.Ceres(problem, "UNKNOWN_ALGORITHM")
simpleObjective = ot.SymbolicFunction(["x1", "x2"], ["x1^2+x2^2"])
with ott.assert_raises(Exception):
    ot.Ceres(ot.OptimizationProblem(ot.SymbolicFunction(["x"], ["x", "2*x"])))
with ott.assert_raises(Exception):
    ot.Ceres(
        ot.NearestPointProblem(
            ot.SymbolicFunction(["x1", "x2"], ["x1^2+x2^2"]), 1.0
        )
    )
boundedProblem = ot.OptimizationProblem(simpleObjective)
boundedProblem.setBounds(ot.Interval([-1.0] * 2, [1.0] * 2))
with ott.assert_raises(Exception):
    ot.Ceres(boundedProblem, "LBFGS")
with ott.assert_raises(Exception):
    ot.Ceres(boundedProblem, "LEVENBERG_MARQUARDT")
constrainedProblem = ot.OptimizationProblem(simpleObjective)
constrainedProblem.setInequalityConstraint(
    ot.SymbolicFunction(["x1", "x2"], ["x1+x2-1.0"])
)
with ott.assert_raises(Exception):
    ot.Ceres(constrainedProblem)
integerProblem = ot.OptimizationProblem(simpleObjective)
integerProblem.setVariablesType(
    [ot.OptimizationProblemImplementation.INTEGER] * 2
)
with ott.assert_raises(Exception):
    ot.Ceres(integerProblem, "LBFGS")
badStart = ot.Ceres(ot.OptimizationProblem(simpleObjective), "LBFGS")
badStart.setStartingPoint([0.0])
with ott.assert_raises(Exception):
    badStart.run()
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("ceres.xml"))
study.add("algo", algo)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("ceres.xml"))
loadedStudy.load()
assert loadedStudy.hasObject("algo")

os.remove("ceres.xml")
