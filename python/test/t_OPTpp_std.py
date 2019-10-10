#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m
import sys

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(3)

def progress(percent):
    sys.stderr.write('-- progress=' + str(percent) + '%\n')

def stop():
    sys.stderr.write('-- stop?\n')
    return False

# rosenbrock x*=(1,1), x*=(0.7864, 0.6177) on unit disk
dim = 2
f = ot.SymbolicFunction(['x1', 'x2'], ['1+100*(x2-x1^2)^2+(1-x1)^2'])
startingPoint = [1e-3] * dim
bounds = ot.Interval([-1.5] * dim, [1.5] * dim)
algoNames = ot.OPTpp.GetAlgorithmNames()
print(algoNames)

for algoName in algoNames:
    # OptBaNewton freezes or does not move on linux
    # OptBCNewton/OptNewton/OptFDNewton/OptConstrFDNewton goes to nans
    # OptConstrNewton yields inconsistent results on mingw/osx
    # OptDHNIPS yields inconsistent results on mingw/osx
    # OptPDS goes very far
    # OptNewton goes to nan on osx, does not move on linux
    # OptNIPS/OptFDNIPS Singular Jacobian exits if newmat11/include.h defines SimulateExceptions (default), best to use UseExceptions
    if 'OptBaNewton' in algoName or 'OptBCNewton' in algoName or 'OptNewton' in algoName or 'OptFDNewton' in algoName  or 'OptConstrFDNewton' in algoName or 'OptConstrNewton' in algoName or 'OptDHNIPS' in algoName or 'OptPDS' in algoName or 'OptNIPS' in algoName or 'OptFDNIPS' in algoName:
        continue

    algo = ot.OPTpp(algoName)

    for minimization in [False, True]:
        for inequality in [False, True]:
            for equality in [False, True]:
                for bound in [False, True]:
                    print('algo=', algoName, 'minimization=', minimization, 'bounds=', bound, 'inequality=', inequality, 'equality=', equality)
                    problem = ot.OptimizationProblem(f)
                    problem.setMinimization(minimization)
                    if inequality:
                        # x1^2+x2^2 <= 1
                        problem.setInequalityConstraint(ot.SymbolicFunction(
                            ['x1', 'x2'], ['1-x1^2-x2^2']))
                    if equality:
                        # x1 = x2
                        problem.setEqualityConstraint(ot.SymbolicFunction(
                            ['x1', 'x2'], ['x1-x2']))
                    if bound:
                        problem.setBounds(bounds)

                    try:
                        algo.setProblem(problem)
                    except:
                        print('-- Not supported')
                        continue
                    #algo.setMaximumEvaluationNumber(100)
                    algo.setStartingPoint(startingPoint)
                    try:
                        algo.run()
                    except Exception as e:
                        print('-- ', e)
                        continue
                    result = algo.getResult()
                    print('x^=', result.getOptimalPoint(), 'y^=', result.getOptimalValue())

## FORM
f = ot.SymbolicFunction(
    ["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])
dim = f.getInputDimension()
mean = [50.0, 1.0, 10.0, 5.0]
sigma = ot.Point(dim, 1.0)
R = ot.IdentityMatrix(dim)
distribution = ot.Normal(mean, sigma, R)
vect = ot.RandomVector(distribution)
output = ot.CompositeRandomVector(f, vect)
event = ot.ThresholdEvent(output, ot.Less(), -3.0)
solver = ot.OPTpp('OptQNIPS')
solver.setMaximumEvaluationNumber(400)
solver.setMaximumAbsoluteError(1.0e-10)
solver.setMaximumRelativeError(1.0e-10)
solver.setMaximumResidualError(1.0e-10)
solver.setMaximumConstraintError(1.0e-10)
algo = ot.FORM(solver, event, mean)
algo.run()
result = algo.getResult()
print('beta=%.6f' % result.getGeneralisedReliabilityIndex())
