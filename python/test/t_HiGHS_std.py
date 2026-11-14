#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# inspired from highs example: https://github.com/ERGO-Code/HiGHS/blob/master/examples/call_highs_from_cpp.cpp
# min f = 1.1*x_0 + x_1
# ineq:
#        x_1 <= 7
#   5 <= x_0 + 2x_1 <= 15
#   6 <= 3x_0 + 2x_1
# bounds:
#   0 <= x_0 <= 4; 1 <= x_1
ot.ResourceMap.AddAsBool("HiGHS-output_flag", True)
bounds = ot.Interval([0.0, 1.0], [4.0, 1e30], [True, True], [True, False])
cost = [1.1, 1.0]
A = ot.Matrix([[0.0, 1.0], [1.0, 2.0], [3.0, 2.0]])
cb = ot.Interval([-1e9, 5.0, 6.0], [7.0, 15.0, 1e30], [False, True, True], [True, True, False])
problem = otexp.LinearProblem(cost, bounds, A, cb)
print(problem)

# algorithm names
names = otexp.HiGHS.GetAlgorithmNames()
print(names)
assert "choose" in names
assert "simplex" in names
assert "ipm" in names
assert "pdlp" in names

# invalid algorithm name
with ott.assert_raises(TypeError):
    otexp.HiGHS(problem, "nosuch")
algo = otexp.HiGHS(problem)
with ott.assert_raises(TypeError):
    algo.setAlgorithmName("nosuch")

# constraint functions cannot be mapped to the HiGHS model: only the linear
# constraints given to setLinearConstraint are supported
problemConstraint = otexp.LinearProblem(cost, bounds, A, cb)
problemConstraint.setInequalityConstraint(
    ot.SymbolicFunction(["x0", "x1"], ["x0 + x1"])
)
with ott.assert_raises(TypeError):
    otexp.HiGHS(problemConstraint)

sol = {
    ot.OptimizationProblemImplementation.CONTINUOUS: [0.5, 2.25],
    ot.OptimizationProblemImplementation.INTEGER: [0.0, 3.0],
}
for vtype in [
    ot.OptimizationProblemImplementation.CONTINUOUS,
    ot.OptimizationProblemImplementation.INTEGER,
]:
    problem.setVariablesType([vtype] * 2)
    for solver in ["choose", "simplex", "ipm"]:
        if vtype != ot.OptimizationProblemImplementation.CONTINUOUS and solver != "choose":
            # discrete problems are solved by the MIP solver
            with ott.assert_raises(TypeError):
                otexp.HiGHS(problem, solver)
            continue
        algo = otexp.HiGHS(problem, solver)
        assert algo.getAlgorithmName() == solver, "algorithm name"
        print(algo)
        algo.run()
        result = algo.getResult()
        assert result.getStatusMessage() == "Optimal", "model status"
        print(result)
        assert result.getStatus() == ot.OptimizationResult.SUCCESS, "status"
        assert result.getIterationNumber() < 1000000, "iteration number"
        x = result.getOptimalPoint()
        ott.assert_almost_equal(x, sol[vtype])
        # duals are only available on continuous problems
        if vtype == ot.OptimizationProblemImplementation.CONTINUOUS:
            duals = algo.getDualPoint()
            assert len(duals) == 3, "dual dimension"
            costs = algo.getReducedCosts()
            assert len(costs) == 2, "reduced costs dimension"
            values = algo.getConstraintValues()
            assert len(values) == 3, "constraint values dimension"
            # constraint values match A*x
            ax = [sum(A[i, j] * x[j] for j in range(2)) for i in range(3)]
            ott.assert_almost_equal(values, ax)
            # per-solver breakdown sums to the result iteration number
            total = (
                algo.getSimplexIterationNumber()
                + algo.getIPMIterationNumber()
                + algo.getCrossoverIterationNumber()
                + algo.getPDLPIterationNumber()
                + algo.getQPIterationNumber()
            )
            assert total == result.getIterationNumber(), "iteration breakdown"
        else:
            with ott.assert_raises(TypeError):
                algo.getDualPoint()
            with ott.assert_raises(TypeError):
                algo.getReducedCosts()

# setter after construction
problem.setVariablesType(
    [ot.OptimizationProblemImplementation.CONTINUOUS] * 2
)
algo = otexp.HiGHS(problem)
algo.setAlgorithmName("simplex")
assert algo.getAlgorithmName() == "simplex", "algorithm name"
algo.run()
ott.assert_almost_equal(
    algo.getResult().getOptimalPoint(), sol[ot.OptimizationProblemImplementation.CONTINUOUS]
)

# the constant term shifts the optimal value, not the optimal point
problem.setConstant(10.0)
assert problem.getConstant() == 10.0
algoConstant = otexp.HiGHS(problem, "simplex")
algoConstant.run()
resultConstant = algoConstant.getResult()
ott.assert_almost_equal(
    resultConstant.getOptimalPoint(), sol[ot.OptimizationProblemImplementation.CONTINUOUS]
)
ott.assert_almost_equal(resultConstant.getOptimalValue(), [1.1 * 0.5 + 2.25 + 10.0])
problem.setConstant(0.0)

# a rejected setter leaves the previous algorithm name unchanged
problemD = otexp.LinearProblem(cost, bounds, A, cb)
problemD.setVariablesType([ot.OptimizationProblemImplementation.INTEGER] * 2)
algoD = otexp.HiGHS(problemD)
with ott.assert_raises(TypeError):
    algoD.setAlgorithmName("simplex")
assert algoD.getAlgorithmName() == "choose", "algorithm name"

# infeasible problem: x >= 2, x <= 1
bounds2 = ot.Interval([0.0], [1.0])
A2 = ot.Matrix([[1.0]])
cb2 = ot.Interval([2.0], [1e30], [True], [False])
problem2 = otexp.LinearProblem([1.0], bounds2, A2, cb2)
algo2 = otexp.HiGHS(problem2)
algo2.setCheckStatus(False)
algo2.run()
assert algo2.getResult().getStatus() != ot.OptimizationResult.SUCCESS, "status"
assert len(algo2.getResult().getStatusMessage()) > 0, "model status"
print(algo2.getResult().getStatusMessage())
algo3 = otexp.HiGHS(problem2)
with ott.assert_raises(Exception):
    algo3.run()

# unsupported problems are rejected
nonlinear = ot.OptimizationProblem(ot.SymbolicFunction(["x"], ["x^2"]))
with ott.assert_raises(TypeError):
    otexp.HiGHS(nonlinear)
multi = ot.OptimizationProblem(ot.SymbolicFunction(["x"], ["x", "2*x"]))
with ott.assert_raises(TypeError):
    otexp.HiGHS(multi)

# problem without linear constraints
problem3 = otexp.LinearProblem(
    [1.0], ot.Interval([0.0], [1.0]), ot.Matrix(0, 1), ot.Interval()
)
algo8 = otexp.HiGHS(problem3, "simplex")
assert "simplex" in repr(algo8), "repr"
algo8.run()
ott.assert_almost_equal(algo8.getResult().getOptimalPoint(), [0.0])
# no linear constraints means no constraint values
with ott.assert_raises(TypeError):
    algo8.getConstraintValues()

# accessors before run
algo4 = otexp.HiGHS(problem)
assert algo4.getResult().getStatusMessage() == "", "model status"
for accessor in [
    algo4.getDualPoint,
    algo4.getReducedCosts,
    algo4.getConstraintValues,
    algo4.getSimplexIterationNumber,
    algo4.getIPMIterationNumber,
    algo4.getCrossoverIterationNumber,
    algo4.getPDLPIterationNumber,
    algo4.getQPIterationNumber,
]:
    with ott.assert_raises(TypeError):
        accessor()

# default constructor then setProblem
algo5 = otexp.HiGHS("simplex")
assert algo5.getAlgorithmName() == "simplex", "algorithm name"
algo5.setProblem(problem)
algo5.run()
ott.assert_almost_equal(
    algo5.getResult().getOptimalPoint(),
    sol[ot.OptimizationProblemImplementation.CONTINUOUS],
)

# iteration limit maps to MAXIMUMCALLS
algo6 = otexp.HiGHS(problem)
algo6.setMaximumIterationNumber(0)
algo6.setCheckStatus(False)
algo6.run()
assert algo6.getResult().getStatus() == ot.OptimizationResult.MAXIMUMCALLS, "status"
assert len(algo6.getResult().getStatusMessage()) > 0, "model status"

# progress/stop callbacks: random dense LP large enough that the solver
# loop iterates and fires the interrupt callbacks
ot.RandomGenerator.SetSeed(0)
normal = ot.Normal()
n_vars = 60
n_cons = 30
cost_cb = normal.getSample(n_vars).asPoint()
bounds_cb = ot.Interval([0.0] * n_vars, [1.0] * n_vars)
A_cb = ot.Matrix(n_cons, n_vars)
for i in range(n_cons):
    row = normal.getSample(n_vars).asPoint()
    for j in range(n_vars):
        A_cb[i, j] = row[j]
upper_cb = [sum(abs(A_cb[i, j]) for j in range(n_vars)) * 0.5 + 1.0 for i in range(n_cons)]
cb_cb = ot.Interval([-1e30] * n_cons, upper_cb, [False] * n_cons, [True] * n_cons)
problem_cb = otexp.LinearProblem(cost_cb, bounds_cb, A_cb, cb_cb)


def no_stop():
    return False


def stop_now():
    return True


def check_monotonic(progress_calls):
    assert len(progress_calls) > 0, "progress callback was not invoked"
    assert all(0.0 <= p <= 100.0 for p in progress_calls), "progress out of range"
    assert all(b >= a for a, b in zip(progress_calls, progress_calls[1:])), "progress not monotonic"


for solver in ["simplex", "ipm"]:
    progress_calls = []

    def progress(p):
        progress_calls.append(p)

    algo_cb = otexp.HiGHS(problem_cb, solver)
    algo_cb.setProgressCallback(progress)
    algo_cb.setStopCallback(no_stop)
    algo_cb.run()
    assert algo_cb.getResult().getStatus() == ot.OptimizationResult.SUCCESS, "status"
    check_monotonic(progress_calls)

stop_algo = otexp.HiGHS(problem_cb, "simplex")
stop_algo.setCheckStatus(False)
stop_algo.setStopCallback(stop_now)
stop_algo.run()
assert stop_algo.getResult().getStatus() == ot.OptimizationResult.INTERRUPTION, "status"
