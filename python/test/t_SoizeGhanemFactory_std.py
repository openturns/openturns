#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

marginals = [ot.Normal(), ot.Uniform()]
R = ot.CorrelationMatrix(2, [1.0, 0.5, 0.5, 1.0])
copula = ot.NormalCopula(R)
factories = [
    ot.SoizeGhanemFactory(ot.JointDistribution(marginals)),
    ot.SoizeGhanemFactory(ot.JointDistribution(marginals, copula), False),
    ot.SoizeGhanemFactory(ot.JointDistribution(marginals, copula), True),
]
x = [0.5] * 2
kMax = 5
ot.ResourceMap.SetAsUnsignedInteger("IteratedQuadrature-MaximumSubIntervals", 2048)
ot.ResourceMap.SetAsScalar("IteratedQuadrature-MaximumError", 1.0e-6)
for soize in factories:
    distribution = soize.getMeasure()
    functions = list()
    for k in range(kMax):
        functions.append(soize.build(k))
    M = ot.SymmetricMatrix(kMax)
    for m in range(kMax):
        for n in range(m + 1):

            def wrapper(x):
                return functions[m](x) * functions[n](x)[0] * distribution.computePDF(x)

            kernel = ot.PythonFunction(distribution.getDimension(), 1, wrapper)
            value = ot.IteratedQuadrature().integrate(kernel, distribution.getRange())[
                0
            ]
            if abs(value) >= 1.0e-6:
                M[m, n] = value
    ott.assert_almost_equal(M, ot.IdentityMatrix(kMax), 1e-5, 0.0)

# Test isTensorProduct(), case True
sgTensorProduct = ot.SoizeGhanemFactory(ot.JointDistribution(marginals))
assert sgTensorProduct.isTensorProduct()
# Test isTensorProduct(), case False
sgTensorProduct = ot.SoizeGhanemFactory(ot.JointDistribution(marginals, copula))
assert not sgTensorProduct.isTensorProduct()

# Test getMarginal
basis = ot.SoizeGhanemFactory(ot.JointDistribution(marginals))
basisMarginal = basis.getMarginal([0])
for i in range(20):
    function = basisMarginal.build(i)
