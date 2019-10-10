#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m

ot.TESTPREAMBLE()


def hyperplane(coefs):
    """
    Generate a linear NMF from its coefficients
    """
    dim = len(coefs)
    constant = [0.]
    center = [0.] * len(coefs)
    linear = ot.Matrix(1, dim)
    for i in range(dim):
        linear[0, i] = coefs[i]
    function = ot.LinearFunction(center, constant, linear)
    return function


for dim in [3, 5, 8][1:2]:
    functions = []
    linears = []

    linear = [1.] * dim
    linears.append(linear)

    function = hyperplane(linear)
    function.setName('H1')
    functions.append(function)

    linear = list(linear)
    linear[0] = 0.25
    linears.append(linear)
    function = hyperplane(linear)
    function.setName('H2')
    functions.append(function)

    linear = list(linear)
    linear = [0.] * dim
    linear[dim - 1] = 1.
    linears.append(linear)
    function = hyperplane(linear)
    function.setName('H3')
    functions.append(function)

    for ih in range(len(functions)):
        function = functions[ih]

        distribution = ot.Normal(dim)
        randomVector = ot.RandomVector(distribution)
        composite = ot.CompositeRandomVector(function, randomVector)

        for pft in [1e-4, 1e-6, 1e-8][1:2]:

            k = ot.Normal().computeQuantile(pft)[
                0] * ot.Point(linears[ih]).norm()
            event = ot.ThresholdEvent(composite, ot.Less(), k)

            print('--------------------')
            print('model H' + str(ih) + ' dim=%d' %
                  dim, 'pft=%.2e' % pft, 'k=%g' % k)

            for n in [100, 1000][1:]:
                for gamma1 in [0.25, 0.5, 0.75][1:2]:
                    # algo = ot.MonteCarlo(event)
                    # algo.setMaximumOuterSampling(100*n)
                    # algo.setMaximumCoefficientOfVariation(-1.)
                    # algo.run()
                    # result = algo.getResult()
                    # print result
                    algo = ot.AdaptiveDirectionalSampling(event)
                    algo.setMaximumOuterSampling(n)
                    algo.setGamma([gamma1, 1.0 - gamma1])
                    calls0 = function.getEvaluationCallsNumber()
                    algo.run()
                    calls = function.getEvaluationCallsNumber() - calls0
                    result = algo.getResult()
                    pf = result.getProbabilityEstimate()
                    var = result.getVarianceEstimate()
                    cov = result.getCoefficientOfVariation()
                    print(result, end=' ')
                    print('callsNumber=%d' %
                          calls + ' gamma=' + str(algo.getGamma()))
