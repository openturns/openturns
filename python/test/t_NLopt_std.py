#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m


def printPoint(point, digits):
    oss = "["
    eps = pow(0.1, digits)
    format = "%." + str(digits) + "f"
    for i in range(point.getDimension()):
        if i == 0:
            sep = ""
        else:
            sep = ","
        if m.fabs(point[i]) < eps:
            oss += sep + format % m.fabs(point[i])
        else:
            oss += sep + format % point[i]
        sep = ","
    oss += "]"
    return oss


# bounds
linear = ot.SymbolicFunction(
    ['x1', 'x2', 'x3', 'x4'], ['x1+2*x2-3*x3+4*x4'])

dim = 4
startingPoint = [0.0] * dim

bounds = ot.Interval([-3.] * dim, [5.] * dim)
algoNames = ot.NLopt.GetAlgorithmNames()

for algoName in algoNames:

    # STOGO might not be enabled
    # NEWUOA nan/-nan
    # COBYLA crashes on squeeze
    # ESCH not same results with 2.4.1
    if 'STOGO' in algoName or 'NEWUOA' in algoName or 'COBYLA' in algoName or 'ESCH' in algoName:
        print('-- Skipped: algo=', algoName)
        continue

    algo = ot.NLopt(algoName)

    for minimization in [True, False]:
        for inequality in [True, False]:
            for equality in [True, False]:
                problem = ot.OptimizationProblem(
                    linear, ot.Function(), ot.Function(), bounds)
                problem.setMinimization(minimization)
                if inequality:
                    # x3 <= x1
                    problem.setInequalityConstraint(ot.SymbolicFunction(
                        ['x1', 'x2', 'x3', 'x4'], ['x1-x3']))
                if equality:
                    # x4 = 2
                    problem.setEqualityConstraint(ot.SymbolicFunction(
                        ['x1', 'x2', 'x3', 'x4'], ['x4-2']))
                ot.NLopt.SetSeed(0)
                try:
                    algo.setProblem(problem)
                except:
                    print('-- Not supported: algo=', algoName,
                          'inequality=', inequality, 'equality=', equality)
                    continue
                algo.setMaximumEvaluationNumber(5000)
                algo.setStartingPoint(startingPoint)
                # algo.setInitialStep([0.1] * dim)
                localAlgo = ot.NLopt('LD_MMA')
                algo.setLocalSolver(localAlgo)
                print('algo=', algo)
                try:
                    algo.run()
                except Exception as e:
                    print('-- ', e)
                    continue
                result = algo.getResult()
                print('x^=', printPoint(
                result.getOptimalPoint(), 3))


# FORM
f = ot.SymbolicFunction(
    ["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])
dim = f.getInputDimension()
mean = [50.0, 1.0, 10.0, 5.0]
sigma = ot.Point(dim, 1.0)
R = ot.IdentityMatrix(dim)
distribution = ot.Normal(mean, sigma, R)
vect = ot.RandomVector(distribution)
output = ot.RandomVector(f, vect)
myEvent = ot.Event(output, ot.Less(), -3.0)
solver = ot.NLopt('LD_AUGLAG')
solver.setMaximumIterationNumber(400)
solver.setMaximumAbsoluteError(1.0e-10)
solver.setMaximumRelativeError(1.0e-10)
solver.setMaximumResidualError(1.0e-10)
solver.setMaximumConstraintError(1.0e-10)
algo = ot.FORM(solver, myEvent, mean)
algo.run()
result = algo.getResult()
print('generalized reliability index=%.6f' %
      result.getGeneralisedReliabilityIndex())
