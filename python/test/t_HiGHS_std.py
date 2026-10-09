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
bounds = ot.Interval([0.0, 1.0], [4.0, 1e30])
cost = [1.1, 1.0]
A = ot.Matrix([[0.0, 1.0], [1.0, 2.0], [3.0, 2.0]])
cb = ot.Interval([-1e9, 5.0, 6.0], [7.0, 15.0, 1e30])
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
cb2 = ot.Interval([2.0], [1e30])
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
for accessor in [algo4.getDualPoint, algo4.getReducedCosts, algo4.getConstraintValues]:
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
