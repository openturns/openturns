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
    ott.assert_almost_equal(adaCoeff, refCoeff)

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
    ott.assert_almost_equal(adaCoeff, refCoeff)
