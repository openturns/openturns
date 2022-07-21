#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from math import exp

ot.TESTPREAMBLE()

ot.Log.Show(ot.Log.ALL)


def test_solver(solver):
    relEps = solver.getRelativeError()
    absEps = solver.getAbsoluteError()

    def test_f1(x):
        y = exp(x[0]) - 1.9151695967140057e-174
        return [y]
    f1 = ot.PythonFunction(1, 1, test_f1)
    root = solver.solve(f1, 0.0, -450.0, -350.0)
    ott.assert_almost_equal(root, -400.0, relEps, absEps)

    def test_f2(x):
        y = exp(x[0]) - 5.221469689764144e+173
        return [y]
    f2 = ot.PythonFunction(1, 1, test_f2)
    root = solver.solve(f2, 0.0, 350.0, 450.0)
    ott.assert_almost_equal(root, 400.0, relEps, absEps)


# 1) Bisection
algo = ot.Bisection()
test_solver(algo)

# 2) Brent
algo = ot.Brent()
test_solver(algo)

# 3) Secant
algo = ot.Secant()
test_solver(algo)
