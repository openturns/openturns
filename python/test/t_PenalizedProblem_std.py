#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
from openturns.testing import assert_almost_equal


def f_py(x):
    if x[0] < 0.0:
        raise RuntimeError("negative input")
    return [x[0] ** 0.5]


penalty = 1.0e6
f = ot.PythonFunction(1, 1, f_py)
problem = ot.OptimizationProblem(f)
problem.setMinimization(True)
penalizedProblem = otexp.PenalizedProblem(problem, penalty)

# penalized objective returns penalty on failure (minimization)
assert_almost_equal(
    penalizedProblem.getObjective()([4.0]), [2.0], 1e-12, 0.0
)
assert_almost_equal(
    penalizedProblem.getObjective()([-1.0]), [penalty], 0.0, 0.0
)

# maximization flips the sign
problemMax = ot.OptimizationProblem(f)
problemMax.setMinimization(False)
penalizedMax = otexp.PenalizedProblem(problemMax, penalty)
assert_almost_equal(penalizedMax.getObjective()([-1.0]), [-penalty], 0.0, 0.0)

# multi-objective penalizes each output according to flags


def g_py(x):
    if x[0] < 0.0:
        raise RuntimeError("negative input")
    return [x[0] ** 0.5, x[0] ** 0.5]


g = ot.PythonFunction(1, 2, g_py)
multiProblem = ot.OptimizationProblem(g)
multiProblem.setMinimization(True, 0)
multiProblem.setMinimization(False, 1)
penalizedMulti = otexp.PenalizedProblem(multiProblem, penalty)
assert_almost_equal(
    penalizedMulti.getObjective()([-1.0]), [penalty, -penalty], 0.0, 0.0
)
assert_almost_equal(
    penalizedMulti.getObjective()([4.0]), [2.0, 2.0], 1e-12, 0.0
)

# accessors
assert_almost_equal(penalizedProblem.getPenalizedValue(), penalty, 1e-12, 0.0)
penalizedProblem.setPenalizedValue(123.0)
assert_almost_equal(penalizedProblem.getObjective()([-1.0]), [123.0], 0.0, 0.0)
penalizedProblem.setPenalizedValue(penalty)

# constraints/bounds are preserved
bounds = ot.Interval([-5.0], [5.0])
problem.setBounds(bounds)
penalizedBounds = otexp.PenalizedProblem(problem, penalty)
assert penalizedBounds.hasBounds()
assert_almost_equal(
    penalizedBounds.getBounds().getLowerBound(), [-5.0], 1e-12, 0.0
)

# analytic gradient/hessian are wrapped and return zeros on failure


def df_py(x):
    if x[0] < 0.0:
        raise RuntimeError("negative input")
    return [[0.5 / x[0] ** 0.5]]


def d2f_py(x):
    if x[0] < 0.0:
        raise RuntimeError("negative input")
    return [[[-0.25 / x[0] ** 1.5]]]


fa = ot.PythonFunction(1, 1, f_py, gradient=df_py, hessian=d2f_py)
analyticProblem = otexp.PenalizedProblem(ot.OptimizationProblem(fa), penalty)
analyticObjective = analyticProblem.getObjective()
gradClassName = analyticObjective.getGradient().getImplementation()
assert gradClassName.getClassName() == "PenalizedGradient"
hessClassName = analyticObjective.getHessian().getImplementation()
assert hessClassName.getClassName() == "PenalizedHessian"
assert_almost_equal(
    analyticProblem.getObjective().gradient([4.0])[0, 0], 0.25, 1e-12, 0.0
)
assert_almost_equal(
    analyticProblem.getObjective().gradient([-1.0]),
    ot.Matrix(1, 1),
    0.0,
    0.0,
)
assert_almost_equal(
    analyticProblem.getObjective().hessian([-1.0]),
    ot.SymmetricTensor(1, 1),
    0.0,
    0.0,
)

# derivatives are zeroed wherever only the evaluation throws


def df_ok_py(x):
    return [[0.5 / abs(x[0]) ** 0.5]]


def d2f_ok_py(x):
    return [[[-0.25 / abs(x[0]) ** 1.5]]]


fb = ot.PythonFunction(1, 1, f_py, gradient=df_ok_py, hessian=d2f_ok_py)
coordinatedProblem = otexp.PenalizedProblem(
    ot.OptimizationProblem(fb), penalty
)
coordinatedObjective = coordinatedProblem.getObjective()
assert_almost_equal(
    coordinatedObjective.gradient([4.0])[0, 0], 0.25, 1e-12, 0.0
)
assert_almost_equal(
    coordinatedObjective.gradient([-1.0]),
    ot.Matrix(1, 1),
    0.0,
    0.0,
)
assert_almost_equal(
    coordinatedObjective.hessian([-1.0]),
    ot.SymmetricTensor(1, 1),
    0.0,
    0.0,
)
# coordinated wrappers are installed
assert (
    coordinatedObjective.getGradient().getImplementation().getClassName()
    == "PenalizedGradient"
)
assert (
    coordinatedObjective.getHessian().getImplementation().getClassName()
    == "PenalizedHessian"
)

# finite-difference gradient needs no wrapping: evaluated on penalized values
fdProblem = otexp.PenalizedProblem(problem, penalty)
assert (
    fdProblem.getObjective().getGradient().getImplementation().getClassName()
    != "PenalizedGradient"
)

# failed constraints are reported as infeasible


def eq_py(x):
    if x[0] < 0.0:
        raise RuntimeError("negative input")
    return [x[0] - 4.0]


def ineq_py(x):
    if x[0] < 0.0:
        raise RuntimeError("negative input")
    return [x[0] - 1.0]


def dineq_ok_py(x):
    return [[1.0]]


constrained = ot.OptimizationProblem(f)
constrained.setMinimization(True)
constrained.setEqualityConstraint(ot.PythonFunction(1, 1, eq_py))
constrained.setInequalityConstraint(ot.PythonFunction(1, 1, ineq_py))
penalizedConstrained = otexp.PenalizedProblem(constrained, penalty)
assert penalizedConstrained.hasEqualityConstraint()
assert penalizedConstrained.hasInequalityConstraint()
# feasible point: constraints agree with the raw functions
assert_almost_equal(
    penalizedConstrained.getEqualityConstraint()([4.0]), [0.0], 1e-12, 0.0
)
assert_almost_equal(
    penalizedConstrained.getInequalityConstraint()([4.0]), [3.0], 1e-12, 0.0
)
# failure: equality reports +penalty (infeasible since != 0),
# inequality reports -penalty (infeasible since < 0)
assert_almost_equal(
    penalizedConstrained.getEqualityConstraint()([-1.0]), [penalty], 0.0, 0.0
)
assert_almost_equal(
    penalizedConstrained.getInequalityConstraint()([-1.0]),
    [-penalty],
    0.0,
    0.0,
)

# raw problem is preserved
assert_almost_equal(
    penalizedConstrained.getProblem().getEqualityConstraint()([4.0]),
    [0.0],
    1e-12,
    0.0,
)

# setPenalizedValue refreshes constraints too
penalizedConstrained.setPenalizedValue(123.0)
assert_almost_equal(
    penalizedConstrained.getEqualityConstraint()([-1.0]), [123.0], 0.0, 0.0
)
assert_almost_equal(
    penalizedConstrained.getInequalityConstraint()([-1.0]), [-123.0], 0.0, 0.0
)
penalizedConstrained.setPenalizedValue(penalty)

# constraints can be replaced after construction
penalizedConstrained.setInequalityConstraint(ot.PythonFunction(1, 1, ineq_py))
assert_almost_equal(
    penalizedConstrained.getInequalityConstraint()([-1.0]),
    [-penalty],
    0.0,
    0.0,
)
penalizedConstrained.setEqualityConstraint(ot.PythonFunction(1, 1, eq_py))
assert_almost_equal(
    penalizedConstrained.getEqualityConstraint()([-1.0]), [penalty], 0.0, 0.0
)

# optimizer proceeds with penalized constraints (no model throw escapes)
inequalityOnly = ot.OptimizationProblem(f)
inequalityOnly.setMinimization(True)
inequalityOnly.setInequalityConstraint(ot.PythonFunction(1, 1, ineq_py))
penalizedInequality = otexp.PenalizedProblem(inequalityOnly, penalty)
algo = ot.Cobyla(ot.OptimizationProblem(penalizedInequality))
algo.setStartingPoint([0.5])
algo.run()
result = algo.getResult()
assert result.getOptimalValue()[0] < penalty
print("constrained optimal point=", result.getOptimalPoint())

# constraint derivatives are zeroed wherever only the evaluation throws
analyticIneqOnly = ot.OptimizationProblem(f)
analyticIneqOnly.setMinimization(True)
analyticIneqOnly.setInequalityConstraint(
    ot.PythonFunction(1, 1, ineq_py, gradient=dineq_ok_py)
)
penalizedAnalyticIneq = otexp.PenalizedProblem(analyticIneqOnly, penalty)
penalizedIneq = penalizedAnalyticIneq.getInequalityConstraint()
assert penalizedIneq.getGradient().getImplementation().getClassName() == (
    "PenalizedGradient"
)
assert_almost_equal(penalizedIneq([4.0]), [3.0], 1e-12, 0.0)
assert_almost_equal(penalizedIneq([-1.0]), [-penalty], 0.0, 0.0)
assert_almost_equal(penalizedIneq.gradient([4.0])[0, 0], 1.0, 1e-12, 0.0)
assert_almost_equal(
    penalizedIneq.gradient([-1.0]), ot.Matrix(1, 1), 0.0, 0.0
)

# can be used by an optimizer
algo = ot.Cobyla(ot.OptimizationProblem(penalizedProblem))
algo.setStartingPoint([4.0])
algo.run()
result = algo.getResult()
assert result.getOptimalValue()[0] < penalty
print("optimal point=", result.getOptimalPoint())

# check with pagmo (population-based, batch evaluations)
if ot.PlatformInfo.HasFeature("pagmo"):
    ot.RandomGenerator.SetSeed(0)
    pagmoProblem = ot.OptimizationProblem(penalizedBounds)
    pop0 = ot.Uniform(-5.0, 5.0).getSample(20)
    algo = ot.Pagmo(pagmoProblem, "de", pop0)
    algo.setMaximumIterationNumber(5)
    algo.run()
    result = algo.getResult()
    assert result.getOptimalValue()[0] < penalty
    print("pagmo optimal point=", result.getOptimalPoint())
print("ok")
