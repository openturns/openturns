#! /usr/bin/env pythons

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

# Analytical construction */
inputs = ["x", "y"]
formulas = ["y*x-sin(2*x)", "1 + cos(y) + x"]
analytical = ot.SymbolicFunction(inputs, formulas)
algo = ot.LeastSquaresEquationsSolver()
algo.setResidualError(1e-10)
algo.setMaximumCallsNumber(1000)
starting_point = [2.0, 1.0]
print("Solve ", formulas, "= [0,0] for ", inputs)
solution = algo.solve(analytical, starting_point)
print("[x,y] = ", solution)
print("algo=", algo)
ott.assert_almost_equal(analytical(solution), [0, 0], 1e-5, 1e-5)

# Extra coverage (no output to preserve expout)
assert len(repr(algo)) > 0
default_solver = ot.LeastSquaresEquationsSolver()
assert default_solver.getOptimizationAlgorithm().getClassName() == "OptimizationAlgorithm"
algo.setOptimizationAlgorithm(ot.Cobyla())
assert "Cobyla" in repr(algo.getOptimizationAlgorithm())
# incompatible optimizer triggers the fallback solver setup
with ott.assert_raises(Exception):
    algo.solve(analytical, starting_point)
algo.setOptimizationAlgorithm(default_solver.getOptimizationAlgorithm())
# solve with explicit bounds
bounded = algo.solve(
    analytical, starting_point, ot.Interval([-5.0, -5.0], [5.0, 5.0])
)
ott.assert_almost_equal(analytical(bounded), [0, 0], 1e-5, 1e-5)
# bounds of wrong dimension
with ott.assert_raises(Exception):
    algo.solve(analytical, starting_point, ot.Interval([0.0], [5.0]))
# residual threshold not met
strict = ot.LeastSquaresEquationsSolver()
strict.setResidualError(1e-30)
strict.setMaximumCallsNumber(2)
with ott.assert_raises(Exception):
    strict.solve(analytical, starting_point)
# save / load roundtrip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("lses_test.xml"))
study.add("algo", algo)
study.save()
study2 = ot.Study()
study2.setStorageManager(ot.XMLStorageManager("lses_test.xml"))
study2.load()
loaded = ot.LeastSquaresEquationsSolver()
study2.fillObject("algo", loaded)
assert len(repr(loaded)) > 0

os.remove("lses_test.xml")
