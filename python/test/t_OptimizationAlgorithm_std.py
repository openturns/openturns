#! /usr/bin/env python

import openturns as ot
import time

ot.TESTPREAMBLE()

dim = 2
f = ot.SymbolicFunction(["x1", "x2"], ["x1^2+x2^2"])
x0 = [0.5] * dim
bounds = ot.Interval([-1.0, -10.0], [10.0, 1.0])

for minimization in [True, False]:
    for ineq in [False, True]:
        problem = ot.OptimizationProblem(f)
        problem.setBounds(bounds)
        problem.setMinimization(minimization)
        if ineq:
            h = ot.SymbolicFunction(["x1", "x2"], ["5-x1", "x2+5"])
            problem.setInequalityConstraint(h)
        names = ot.OptimizationAlgorithm.GetAlgorithmNames(problem)
        for name in names:
            if "global" in name:
                # slow
                continue
            algo = ot.OptimizationAlgorithm.GetByName(name)
            algo.setProblem(problem)
            algo.setMaximumConstraintError(1e-1)
            algo.setMaximumCallsNumber(1000)
            try:
                algo.setStartingPoint(x0)
            except Exception:
                # multi-obj
                continue
            print(f"minimization={minimization} ineq={ineq} algo={name}")
            algo.run()
            result = algo.getResult()
            x = result.getOptimalPoint()
            y = result.getOptimalValue()
            print(f"x^={x} y^={y}")
            if minimization:
                assert abs(y[0]) < 0.3
            else:
                ref = 25.0 if ineq else 100.0
                assert y[0] > ref


def _exec(x):
    if x[0] < 0.0 or x[0] > 1.0:
        raise ValueError(f"Point {x[0]} not in bounds.")
    c = 1.0 - x[0] ** 2
    return [c]


# check algorithm stays inside bounds
eps = 1e-14
ot.ResourceMap.SetAsScalar("CenteredFiniteDifferenceGradient-DefaultEpsilon", eps)
ot.ResourceMap.SetAsScalar("CenteredFiniteDifferenceHessian-DefaultEpsilon", eps)
# the tolerance prevents FD gradients from stepping outside of [0,1]
# but we still want to keep it as low as possible to detect slightly out of bounds points
bounds = ot.Interval([2.0 * eps], [1.0 - 2.0 * eps])
costFunction = ot.PythonFunction(1, 1, _exec)
problem = ot.OptimizationProblem(costFunction)
problem.setBounds(bounds)
for name in ot.OptimizationAlgorithm.GetAlgorithmNames():
    algo = ot.OptimizationAlgorithm.GetByName(name)
    algo.setMaximumConstraintError(0.0)
    try:
        algo.setProblem(problem)
        startingPoint = [0.5]
        algo.setStartingPoint(startingPoint)
    except Exception:
        # not supported
        continue
    algo.setMaximumCallsNumber(100)
    algo.setMaximumIterationNumber(100)
    print(f"{name}...")
    algo.run()
    assert algo.getResult().getStatus() == ot.OptimizationResult.SUCCESS
    print(f"{name}: OK")


def _exec(X):
    time.sleep(0.2)
    x1, x2 = X
    return [x1**2 + x2**2]


# check algorithm enforces time limit
ot.Log.Show(ot.Log.ALL)
bounds = ot.Interval([-10] * 2, [10] * 2)
costFunction = ot.PythonFunction(2, 1, _exec)
problem = ot.OptimizationProblem(costFunction)
problem.setMinimization(False)
problem.setBounds(bounds)
for name in ot.OptimizationAlgorithm.GetAlgorithmNames():
    if name in ot.Bonmin.GetAlgorithmNames():
        # interruption has to be forced but cannot recover
        continue
    if "AUGLAG" in name:
        # returns XTOL_REACHED
        continue
    if name == "Ipopt":
        # walltime criterion requires ipopt>=3.14
        continue
    algo = ot.OptimizationAlgorithm.GetByName(name)
    try:
        algo.setProblem(problem)
        startingPoint = [0.1] * 2
        algo.setStartingPoint(startingPoint)
    except Exception:
        # not supported
        continue
    algo.setMaximumIterationNumber(50)
    algo.setMaximumCallsNumber(100)
    algo.setMaximumTimeDuration(0.1)
    algo.setCheckStatus(False)
    print(f"{name}...")
    algo.run()
    status = algo.getResult().getStatus()
    msg = algo.getResult().getStatusMessage()
    calls = algo.getResult().getCallsNumber()
    print(f"{name}: {status} {msg} {calls}")
    assert status == ot.OptimizationResult.TIMEOUT, name

# infeasible problem
objective = ot.SymbolicFunction(
    ["x1", "x2", "x3", "x4"], ["x1 + 2 * x2 - 3 * x3 + 4 * x4"]
)
inequality_constraint = ot.SymbolicFunction(["x1", "x2", "x3", "x4"], ["-1.0"])
dim = objective.getInputDimension()
bounds = ot.Interval([-3.0] * dim, [5.0] * dim)
problem = ot.OptimizationProblem(objective)
problem.setMinimization(True)
problem.setInequalityConstraint(inequality_constraint)
problem.setBounds(bounds)
for name in ot.OptimizationAlgorithm.GetAlgorithmNames():
    algo = ot.OptimizationAlgorithm.GetByName(name)
    algo.setCheckStatus(False)
    try:
        algo.setProblem(problem)
        startingPoint = [0.0] * dim
        algo.setStartingPoint(startingPoint)
    except Exception:
        # not supported
        continue
    print(f"{name}...")
    try:
        algo.run()
    except Exception:
        # no feasible point
        pass
    result = algo.getResult()
    status = algo.getResult().getStatus()
    msg = algo.getResult().getStatusMessage()
    calls = algo.getResult().getCallsNumber()
    print(f"{name}: {status} {msg} {calls}")
    assert len(result.getOptimalPoint()) == 0, "should not find point"
    assert status == ot.OptimizationResult.FAILURE, "should return FAILURE"

# stop immediately
rosenbrock = ot.SymbolicFunction(['x1', 'x2'], ['(1-x1)^2+100*(x2-x1^2)^2'])
problem = ot.OptimizationProblem(rosenbrock)
for name in ot.OptimizationAlgorithm.GetAlgorithmNames():
    algo = ot.OptimizationAlgorithm.GetByName(name)
    try:
        algo.setProblem(problem)
        algo.setStartingPoint([0.0] * 2)
    except Exception:
        # not supported
        continue
    print(f"{name}...")
    algo.setMaximumResidualError(1e-3)
    algo.setMaximumCallsNumber(10000)
    if name in list(ot.Bonmin.GetAlgorithmNames()) + ["Ipopt"]:
        algo.setCheckStatus(False)  # optim fails with "Invalid number detected"

    def ask_stop():
        return True

    algo.setStopCallback(ask_stop)
    algo.run()
    result = algo.getResult()
    status = algo.getResult().getStatus()
    msg = algo.getResult().getStatusMessage()
    calls = algo.getResult().getCallsNumber()
    print(f"{name}: {status} {msg} {calls}")
    assert result.getIterationNumber() <= 1
