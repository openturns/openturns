#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m

dim = 4


def flood_model(X):
    L = 5000.  # m
    B = 300.  # m
    Q = X[0]  # m^3.s^-1
    Ks = X[1]  # m^1/3.s^-1
    Zv = X[2]  # m
    Zm = X[3]  # m
    Hd = 0.  # m
    Zb = 55.5  # m
    S = Zv + (Q / (Ks * B * m.sqrt((Zm - Zv) / L))) ** (3. / 5) - (Hd + Zb)
    return [S]

function = ot.PythonFunction(dim, 1, flood_model)

Q_law = ot.TruncatedDistribution(
    ot.Gumbel(1. / 558., 1013., ot.Gumbel.ALPHABETA), 0., ot.TruncatedDistribution.LOWER)
# alpha=1/b, beta=a | you can use Gumbel(a, b, Gumbel.AB) starting from OT 1.2
Ks_law = ot.TruncatedDistribution(
    ot.Normal(30.0, 7.5), 0., ot.TruncatedDistribution.LOWER)
Zv_law = ot.Triangular(49., 50., 51.)
Zm_law = ot.Triangular(54., 55., 56.)
coll = ot.DistributionCollection([Q_law, Ks_law, Zv_law, Zm_law])
distribution = ot.ComposedDistribution(coll)

x = list(map(lambda dist: dist.computeQuantile(0.5)[0], coll))
fx = function(x)

for k in [0.0, 2.0, 5.0, 8.][0:1]:
    randomVector = ot.RandomVector(distribution)
    composite = ot.RandomVector(function, randomVector)

    print('--------------------')
    print('model flood S <', k, 'gamma=', end=' ')
    print('f(', ot.NumericalPoint(x), ')=', fx)

    event = ot.Event(composite, ot.Greater(), k)
    for n in [100, 1000, 5000][1:2]:
        for gamma1 in [0.25, 0.5, 0.75][1:2]:
            algo = ot.MonteCarlo(event)
            algo.setMaximumOuterSampling(100 * n)
            # algo.setMaximumCoefficientOfVariation(-1.)
            algo.run()
            result = algo.getResult()
            print(result)
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
            print('callsNumber=%d' % calls + ' gamma=' + str(algo.getGamma()))
