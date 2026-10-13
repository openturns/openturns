#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import copy
import os

ot.TESTPREAMBLE()

residual = ot.SymbolicFunction(["a", "b"], ["a + b", "a - b"])

# default ctor
default = ot.LeastSquaresProblem()
assert default.hasResidualFunction()
assert default.getDimension() == 0

# residual ctor
problem = ot.LeastSquaresProblem(residual)
assert problem.hasResidualFunction()
assert problem.getResidualFunction() == residual
assert problem.getDimension() == 2
# the objective is 1/2 * ||residual||^2 = (9 + 1) / 2
ott.assert_almost_equal(problem.getObjective()([1.0, 2.0]), [5.0], 1e-14, 1e-14)
ott.assert_almost_equal(
    problem.getObjective().gradient([1.0, 2.0]),
    ot.Matrix([[2.0], [4.0]]),
    1e-12,
    1e-12,
)
# Hessian of 1/2*||r||^2 is J^T.J = 2*I here
hessian = problem.getObjective().hessian([1.0, 2.0])
ott.assert_almost_equal(hessian[0, 0, 0], 2.0, 1e-12, 1e-12)
ott.assert_almost_equal(hessian[1, 1, 0], 2.0, 1e-12, 1e-12)
ott.assert_almost_equal(hessian[0, 1, 0], 0.0, 1e-12, 1e-12)
assert len(repr(problem)) > 0
assert problem == problem
assert not (problem != problem)

# clone

assert copy.copy(problem).getResidualFunction() == residual

# reset the residual function
other = ot.SymbolicFunction(["a", "b"], ["a * b", "a - b"])
problem.setResidualFunction(other)
assert problem.getResidualFunction() == other
ott.assert_almost_equal(problem.getObjective()([1.0, 2.0]), [2.5], 1e-14, 1e-14)

# bounds / minimization flag behave like the base class
bounds = ot.Interval([-5.0, -5.0], [5.0, 5.0])
problem.setBounds(bounds)
assert problem.hasBounds()
problem.setMinimization(False)
assert not problem.isMinimization()
problem.setMinimization(True)
assert problem.isMinimization()

# save/load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("least_squares_problem.xml"))
study.add("problem", problem)
study.save()
loaded_study = ot.Study()
loaded_study.setStorageManager(ot.XMLStorageManager("least_squares_problem.xml"))
loaded_study.load()
loaded = ot.OptimizationProblem()
loaded_study.fillObject("problem", loaded)
assert loaded.hasResidualFunction()
assert loaded.getResidualFunction() == other

os.remove("least_squares_problem.xml")
