#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import sys

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(3)


def progress(percent):
    sys.stderr.write("-- progress=" + str(percent) + "%\n")


def stop():
    sys.stderr.write("-- stop?\n")
    return False


dim = 2
f = ot.SymbolicFunction(["x1", "x2"], ["10-5*(x1-3)^2-7*(x2-2)^2+0.1*(x1+x2)"])
startingPoint = [0.0] * dim
bounds = ot.Interval([-6.0, -6.0], [1.0, 7.0])
algoNames = ot.NLopt.GetAlgorithmNames()

for algoName in algoNames:
    algo = ot.NLopt(algoName)
    if algoName == "GN_ISRES":
        algo.setMaximumConstraintError(1e-2)
    for minimization in [False, True]:
        for inequality in [False, True]:
            for equality in [False, True]:
                for bound in [False, True]:
                    if minimization and not bound:
                        continue
                    print(
                        "algo=",
                        algoName,
                        "minimization=",
                        minimization,
                        "bounds=",
                        bound,
                        "inequality=",
                        inequality,
                        "equality=",
                        equality,
                    )
                    problem = ot.OptimizationProblem(f)
                    problem.setMinimization(minimization)
                    if inequality:
                        # x1 <= 2
                        problem.setInequalityConstraint(
                            ot.SymbolicFunction(["x1", "x2"], ["2-x1"])
                        )
                    if equality:
                        # x2 = 4
                        problem.setEqualityConstraint(
                            ot.SymbolicFunction(["x1", "x2"], ["x2-4"])
                        )
                    if bound:
                        problem.setBounds(bounds)
                    ot.NLopt.SetSeed(0)
                    try:
                        algo.setProblem(problem)
                    except Exception:
                        print("-- Not supported")
                        continue
                    algo.setMaximumEvaluationNumber(1000)
                    algo.setStartingPoint(startingPoint)
                    try:
                        algo.run()
                    except Exception as e:
                        print("-- ", e)
                        continue
                    result = algo.getResult()
                    x = result.getOptimalPoint()
                    print("x^=", x, "y^=", result.getOptimalValue())
                    if not inequality and not equality:
                        if not minimization:
                            if not bound:
                                x_ref = [3.0, 2.0]
                            else:
                                x_ref = [1.0, 2.0]
                        else:
                            x_ref = [-6.0, -6.0]
                            # these go to (-6,7)
                            if "NELDERMEAD" in algoName or "SBPLX" in algoName:
                                continue
                        ott.assert_almost_equal(x, x_ref, 4e-1, 1e-2)
                    elif equality:
                        ott.assert_almost_equal(x[1], 4.0, 4e-1, 1e-2)
                    elif inequality:
                        assert x[0] < 2.01, "!x1<=2.0"

# FORM
f = ot.SymbolicFunction(["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])
dim = f.getInputDimension()
mean = [50.0, 1.0, 10.0, 5.0]
sigma = ot.Point(dim, 1.0)
R = ot.IdentityMatrix(dim)
distribution = ot.Normal(mean, sigma, R)
vect = ot.RandomVector(distribution)
output = ot.CompositeRandomVector(f, vect)
event = ot.ThresholdEvent(output, ot.Less(), -3.0)
solver = ot.NLopt("LD_AUGLAG")
solver.setMaximumEvaluationNumber(400)
solver.setMaximumAbsoluteError(1.0e-10)
solver.setMaximumRelativeError(1.0e-10)
solver.setMaximumResidualError(1.0e-10)
solver.setMaximumConstraintError(1.0e-10)
algo = ot.FORM(solver, event, mean)
algo.run()
result = algo.getResult()
beta = result.getGeneralisedReliabilityIndex()
print("beta=%.6f" % beta)
ott.assert_almost_equal(beta, 1.009255)
