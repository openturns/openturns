#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Poisson related functions
# dPoisson
lambdaMin = 0.2
lambdaMax = 5.0
kMax = 10
n1 = 5
for i1 in range(n1):
    lambda_ = lambdaMin + (lambdaMax - lambdaMin) * i1 / (n1 - 1)
    for k in range(kMax):
        print(
            "dPoisson(%.3g" % lambda_,
            ", ",
            k,
            ")=%.4g" % ot.DistFunc.dPoisson(lambda_, k),
        )
# dPoisson
for i1 in range(n1):
    lambda_ = lambdaMin + (lambdaMax - lambdaMin) * i1 / (n1 - 1)
    for k in range(kMax):
        print(
            "logdPoisson(%.3g" % lambda_,
            ", ",
            k,
            ")=%.4g" % ot.DistFunc.logdPoisson(lambda_, k),
        )
# rPoisson
nR = 5
for i1 in range(n1):
    lambda_ = lambdaMin + (lambdaMax - lambdaMin) * i1 / (n1 - 1)
    for iR in range(nR):
        print("rPoisson(%.3g" % lambda_, ")=", ot.DistFunc.rPoisson(lambda_))

# qPoisson
n2 = 20
for tail in [False, True]:
    for i1 in range(n1):
        lambda_ = lambdaMin + (lambdaMax - lambdaMin) * i1 / (n1 - 1)
        for i2 in range(n2):
            p = i2 / n2
            q0 = ot.DistFunc.qPoisson(lambda_, p, tail)
            print(f"qPoisson(lambda={lambda_:.3g}, p={p:.3g}, tail={tail})={q0}")
