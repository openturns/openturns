#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

iMax = 5
collection = [
    ot.Laplace(0.0, 1.0),
    ot.Logistic(0.0, 1.0),
    ot.Normal(0.0, 1.0),
    ot.Normal(1.0, 1.0),
    ot.Rayleigh(1.0),
    ot.Student(22.0),
    ot.Triangular(-1.0, 0.3, 1.0),
    ot.Uniform(-1.0, 1.0),
    ot.Uniform(-1.0, 3.0),
    ot.WeibullMin(1.0, 3.0),
    ot.Beta(1.0, 2.0, -1.0, 1.0),
    ot.Beta(0.5, 0.5, -1.0, 1.0),
    ot.Beta(0.5, 0.5, -2.0, 3.0),
    ot.Gamma(1.0, 3.0),
    ot.Arcsine(),
    ot.UserDefined([[i] for i in range(10)]),
    ot.UserDefined([[i + 0.5] for i in range(10)]),
]
for distribution in collection:
    name = distribution.getClassName()
    polynomialFactory = ot.StandardDistributionPolynomialFactory(
        ot.AdaptiveStieltjesAlgorithm(distribution)
    )
    print("polynomialFactory(", name, "=", polynomialFactory, ")")
    for i in range(iMax):
        print(name, " polynomial(", i, ")=", polynomialFactory.build(i))
    roots = polynomialFactory.getRoots(iMax - 1)
    print(name, " polynomial(", iMax - 1, ") roots=", roots)
    nodes, weights = polynomialFactory.getNodesAndWeights(iMax - 1)
    print(name, " polynomial(", iMax - 1, ") nodes=", nodes, " and weights=", weights)
    M = ot.SymmetricMatrix(iMax)
    for i in range(iMax):
        pI = polynomialFactory.build(i)
        for j in range(i + 1):
            pJ = polynomialFactory.build(j)

            def kernel(x):
                return [pI(x[0]) * pJ(x[0]) * distribution.computePDF(x)]

            integrand = ot.PythonFunction(1, 1, kernel)
            if distribution.isContinuous():
                M[i, j] = ot.GaussKronrod().integrate(integrand, distribution.getRange())[0]
            else:
                x = distribution.getSupport()
                M[i, j] = integrand(x).computeMean()[0] * len(x)
    print("M=\n", M)
    ott.assert_almost_equal(M, ot.IdentityMatrix(iMax), 0.0, 1e-7)
