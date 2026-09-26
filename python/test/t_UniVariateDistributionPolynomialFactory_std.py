#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

iMax = 5
collection = [
    ot.Laplace(0.0, 1.0),
    ot.Logistic(0.0, 1.0),
    ot.Normal(0.0, 1.0),
    ot.Normal(1.0, 2.0),
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
    ot.FiniteDiscreteDistribution([[i] for i in range(10)]),
    ot.FiniteDiscreteDistribution([[i + 0.5] for i in range(10)]),
]
for distribution in collection:
    polynomialFactory = ot.UniVariateDistributionPolynomialFactory(
        ot.AdaptiveStieltjesAlgorithm(distribution)
    )
    M = ot.SymmetricMatrix(iMax)
    for i in range(iMax):
        pI = polynomialFactory.build(i)
        for j in range(i + 1):
            pJ = polynomialFactory.build(j)

            def kernel(x, pI=pI, pJ=pJ, distribution=distribution):
                return [pI(x[0]) * pJ(x[0]) * distribution.computePDF(x)]

            integrand = ot.PythonFunction(1, 1, kernel)
            if distribution.isContinuous():
                M[i, j] = ot.GaussKronrod().integrate(
                    integrand, distribution.getRange()
                )[0]
            else:
                x = distribution.getSupport()
                M[i, j] = integrand(x).computeMean()[0] * len(x)
    ott.assert_almost_equal(M, ot.IdentityMatrix(iMax), 0.0, 1e-5)

# Test UniVariateDistributionPolynomialFactory with distribution constructor
for distribution in collection:
    polynomialFactory = ot.UniVariateDistributionPolynomialFactory(
        distribution
    )
    M = ot.SymmetricMatrix(iMax)
    for i in range(iMax):
        pI = polynomialFactory.build(i)
        for j in range(i + 1):
            pJ = polynomialFactory.build(j)

            def kernel2(x, pI=pI, pJ=pJ, distribution=distribution):
                return [pI(x[0]) * pJ(x[0]) * distribution.computePDF(x)]

            integrand = ot.PythonFunction(1, 1, kernel2)
            if distribution.isContinuous():
                M[i, j] = ot.GaussKronrod().integrate(
                    integrand, distribution.getRange()
                )[0]
            else:
                x = distribution.getSupport()
                M[i, j] = integrand(x).computeMean()[0] * len(x)
    ott.assert_almost_equal(M, ot.IdentityMatrix(iMax), 0.0, 1e-5)

# Verify specific families are detected for common distributions
norm = ot.UniVariateDistributionPolynomialFactory(ot.Normal(2.0, 3.0))
assert norm.getHasSpecificFamily(), "Normal(2,3) should have specific family"
assert (
    norm.getSpecificFamily().getImplementation().getClassName()
    == "HermiteFactory"
)

uni = ot.UniVariateDistributionPolynomialFactory(
    ot.Uniform(-5.0, 10.0)
)
assert uni.getHasSpecificFamily(), "Uniform(-5,10) should have specific family"
assert (
    uni.getSpecificFamily().getImplementation().getClassName()
    == "LegendreFactory"
)

beta = ot.UniVariateDistributionPolynomialFactory(
    ot.Beta(3.0, 4.0, -2.0, 5.0)
)
assert (
    beta.getHasSpecificFamily()
), "Beta(3,4,-2,5) should have specific family"
assert (
    beta.getSpecificFamily().getImplementation().getClassName()
    == "JacobiFactory"
)

gamma = ot.UniVariateDistributionPolynomialFactory(
    ot.Gamma(2.5, 1.5, 0.5)
)
assert (
    gamma.getHasSpecificFamily()
), "Gamma(2.5,1.5,0.5) should have specific family"
assert (
    gamma.getSpecificFamily().getImplementation().getClassName()
    == "LaguerreFactory"
)
# lambda=1.5 (not 1.0) so lam vs 1/lam confusion is visible;
# build() must be consistent with getRoots/getNodesAndWeights delegation
ott.assert_almost_equal(gamma.getA(), 1.5)
ott.assert_almost_equal(gamma.getB(), -1.5 * 0.5)
for n in range(4):
    ott.assert_almost_equal(gamma.build(n).getA(), 1.5)
    ott.assert_almost_equal(gamma.build(n).getB(), -1.5 * 0.5)
    ott.assert_almost_equal(
        gamma.build(n)(0.5), gamma.getSpecificFamily().build(n)(0.5)
    )

arc = ot.UniVariateDistributionPolynomialFactory(ot.Arcsine(-2.0, 3.0))
assert arc.getHasSpecificFamily(), "Arcsine(-2,3) should have specific family"
assert (
    arc.getSpecificFamily().getImplementation().getClassName()
    == "ChebychevFactory"
)
