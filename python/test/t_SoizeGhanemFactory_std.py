#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

marginals = [ot.Normal(), ot.Uniform()]
R = ot.CorrelationMatrix(2, [1.0, 0.5, 0.5, 1.0])
copula = ot.NormalCopula(R)
factories = [ot.SoizeGhanemFactory(ot.ComposedDistribution(marginals)),
             ot.SoizeGhanemFactory(
                 ot.ComposedDistribution(marginals, copula), False),
             ot.SoizeGhanemFactory(ot.ComposedDistribution(marginals, copula), True)]
x = [0.5] * 2
kMax = 5
ot.ResourceMap.SetAsUnsignedInteger(
    "IteratedQuadrature-MaximumSubIntervals", 2048)
ot.ResourceMap.SetAsScalar(
    "IteratedQuadrature-MaximumError",    1.0e-6)
for soize in factories:
    distribution = soize.getMeasure()
    print('SoizeGhanem=', soize)
    functions = list()
    for k in range(kMax):
        functions.append(soize.build(k))
        print('SoizeGhanem(', k, ')=', functions[k].getEvaluation())
        print('SoizeGhanem(', k, ')(', x, '=', functions[k](x))
    M = ot.SymmetricMatrix(kMax)
    for m in range(kMax):
        for n in range(m + 1):
            def wrapper(x):
                return functions[m](x) * functions[n](x)[0] * distribution.computePDF(x)
            kernel = ot.PythonFunction(distribution.getDimension(), 1, wrapper)
            value = ot.IteratedQuadrature().integrate(
                kernel, distribution.getRange())[0]
            if abs(value) >= 1.0e-6:
                M[m, n] = value
    print('M=\n', M)
