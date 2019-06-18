#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)


def clean(polynomial):
    coefficients = polynomial.getCoefficients()
    for i in range(coefficients.getDimension()):
        if abs(coefficients[i]) < 1.0e-8:
            coefficients[i] = 0.0
    return ot.UniVariatePolynomial(coefficients)


iMax = 5
distributionCollection = [ot.Laplace(0.0, 1.0),
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
                          ot.Arcsine()]
for n in range(len(distributionCollection)):
    distribution = distributionCollection[n]
    name = distribution.getClassName()
    polynomialFactory = ot.StandardDistributionPolynomialFactory(
        ot.AdaptiveStieltjesAlgorithm(distribution))
    print("polynomialFactory(", name, "=", polynomialFactory, ")")
    for i in range(iMax):
        print(name, " polynomial(", i, ")=", clean(polynomialFactory.build(i)))
    roots = polynomialFactory.getRoots(iMax - 1)
    print(name, " polynomial(", iMax - 1, ") roots=", roots)
    nodes, weights = polynomialFactory.getNodesAndWeights(iMax - 1)
    print(name, " polynomial(", iMax - 1, ") nodes=",
          nodes, " and weights=", weights)
    M = ot.SymmetricMatrix(iMax)
    for i in range(iMax):
        pI = polynomialFactory.build(i)
        for j in range(i + 1):
            pJ = polynomialFactory.build(j)

            def kernel(x):
                return [pI(x[0]) * pJ(x[0]) * distribution.computePDF(x)]

            M[i, j] = ot.GaussKronrod().integrate(
                ot.PythonFunction(1, 1, kernel), distribution.getRange())[0]
    print("M=\n", M.clean(1.0e-6))
