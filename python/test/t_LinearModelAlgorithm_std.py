#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from math import sin

ot.TESTPREAMBLE()

try:

    # lm build
    print(
        "Fit y ~ 3 - 2 x + 0.05 * sin(x) model using 20 points (sin(x) ~ noise)")
    size = 20
    oneSample = ot.Sample(size, 1)
    twoSample = ot.Sample(size, 1)
    for i in range(size):
        oneSample[i, 0] = 7.0 * sin(-3.5 + (6.5 * i) / (size - 1.0)) + 2.0
        twoSample[i, 0] = -2.0 * oneSample[
            i, 0] + 3.0 + 0.05 * sin(oneSample[i, 0])

    test = ot.LinearModelAlgorithm(oneSample, twoSample)
    result = ot.LinearModelResult(test.getResult())
    print ("trend coefficients = ", result.getTrendCoefficients())

    print("Fit y ~ 1 + 0.1 x + 10 x^2 model using 100 points")
    ot.RandomGenerator.SetSeed(0)
    size = 100
    # Define a linespace from 0 to 10 with size points
    # We use a Box expermient ==> remove 0 & 1 points
    experiment = ot.Box([size - 2])
    X = experiment.generate()
    # X is defined in [0,1]
    X *= [10]
    # Stack X2
    X2 = ot.Sample(X)
    for i in range(size):
        X2[i, 0] = X[i, 0] * X2[i, 0]
    X.stack(X2)
    # Define y
    Y = ot.Sample(size, 1)
    for i in range(size):
        Y[i, 0] = 1.0 +  0.1 * X[i, 0] + 10.0 * \
            X[i, 0] * X[i, 0] + 0.1 * ot.DistFunc.rNormal()
    test = ot.LinearModelAlgorithm(X, Y)
    result = test.getResult()
    print ("trend coefficients = ", result.getTrendCoefficients())

except:
    import sys
    print("t_LinearModelAlgorithm_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
