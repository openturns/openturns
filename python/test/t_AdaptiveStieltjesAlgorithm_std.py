#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


def clean(inPoint):
    dim = inPoint.getDimension()
    for i in range(dim):
        if abs(inPoint[i]) < 1.0e-10:
            inPoint[i] = 0.0
        else:
            inPoint[i] = 1e-4 * int(1e4 * inPoint[i])
    return inPoint


iMax = 5

# Test 1: Uniform -> LegendreFactory (centered, symmetric case)
distribution = ot.Uniform()
reference = ot.LegendreFactory()
algo1 = ot.AdaptiveStieltjesAlgorithm(distribution)
print(algo1)
for i in range(iMax):
    refCoeff = reference.getRecurrenceCoefficients(i)
    adaCoeff = algo1.getRecurrenceCoefficients(i)
    print(
        distribution.getClassName() + " Reference(",
        i,
        ")=",
        clean(refCoeff),
    )
    print(
        distribution.getClassName() + " AdaStielj(",
        i,
        ")=",
        clean(adaCoeff),
    )
    ott.assert_almost_equal(adaCoeff, refCoeff, 1e-5, 1e-8)

# Test 2: Beta(0.5, 2.5, -1, 1) -> JacobiFactory (non-centered case)
distribution = ot.Beta(0.5, 2.5, -1.0, 1.0)
reference = ot.JacobiFactory(0.5, 2.5)
algo1 = ot.AdaptiveStieltjesAlgorithm(distribution)
print(algo1)
for i in range(iMax):
    refCoeff = reference.getRecurrenceCoefficients(i)
    adaCoeff = algo1.getRecurrenceCoefficients(i)
    print(
        distribution.getClassName() + " Reference(",
        i,
        ")=",
        clean(refCoeff),
    )
    print(
        distribution.getClassName() + " AdaStielj(",
        i,
        ")=",
        clean(adaCoeff),
    )
    ott.assert_almost_equal(adaCoeff, refCoeff, 1e-5, 1e-8)

# Test 3: factory built from a user-provided algorithm must produce polynomials
# orthonormal with respect to the original distribution (no double-counting
# of the affine transformation, see PR #3249)
distribution = ot.Triangular(2.0, 3.0, 5.0)
factory = ot.UniVariateDistributionPolynomialFactory(
    ot.AdaptiveStieltjesAlgorithm(distribution)
)
print(
    "factory a=",
    factory.getA(),
    "b=",
    factory.getB(),
    "hasSpecificFamily=",
    factory.getHasSpecificFamily(),
)
ott.assert_almost_equal(factory.getA(), 1.0)
ott.assert_almost_equal(factory.getB(), 0.0)
degree_max = 4
gram = ot.SymmetricMatrix(degree_max)
for i in range(degree_max):
    pI = factory.build(i)
    for j in range(i + 1):
        pJ = factory.build(j)

        def kernel(x, pI=pI, pJ=pJ, dist=distribution):
            return [pI(x[0]) * pJ(x[0]) * dist.computePDF(x)]

        gram[i, j] = ot.GaussKronrod().integrate(
            ot.PythonFunction(1, 1, kernel), distribution.getRange()
        )[0]
ott.assert_almost_equal(gram, ot.IdentityMatrix(degree_max), 0.0, 1e-8)
print("orthonormality wrt the original distribution: ok")
