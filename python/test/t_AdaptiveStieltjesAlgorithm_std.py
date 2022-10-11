#! /usr/bin/env python

import openturns as ot

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
# First test
distribution = ot.Uniform()
algo0 = ot.LegendreFactory()
algo1 = ot.AdaptiveStieltjesAlgorithm(distribution)
print(algo1)
for i in range(iMax):
    print(
        distribution.getClassName() + " Reference(",
        i,
        ")=",
        clean(algo0.getRecurrenceCoefficients(i)),
    )
    print(
        distribution.getClassName() + " AdaStielj(",
        i,
        ")=",
        clean(algo1.getRecurrenceCoefficients(i)),
    )

# Second test
distribution = ot.Beta(0.5, 2.5, -1.0, 1.0)
algo0 = ot.JacobiFactory(0.5, 3.0, 1)
algo1 = ot.AdaptiveStieltjesAlgorithm(distribution)
print(algo1)
for i in range(iMax):
    print(
        distribution.getClassName() + " Reference(",
        i,
        ")=",
        clean(algo0.getRecurrenceCoefficients(i)),
    )
    print(
        distribution.getClassName() + " AdaStielj(",
        i,
        ")=",
        clean(algo1.getRecurrenceCoefficients(i)),
    )
