#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# NonCentralChiSquare related functions
# dNonCentralChiSquare
nuMin = 0.2
nuMax = 5.0
n1 = 5
lambdaMin = 0.2
lambdaMax = 5.0
n2 = 5
xMin = 0.1
xMax = 0.9
nX = 5
for i1 in range(n1):
    nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
    for i2 in range(n2):
        lambda_ = lambdaMin + (lambdaMax - lambdaMin) * i2 / (n2 - 1)
        for iX in range(nX):
            x = xMin + (xMax - xMin) * iX / (nX - 1)
            print(
                "dNonCentralChiSquare(",
                nu,
                ", ",
                lambda_,
                ",  %.12g" % x,
                ")=%.6g" % ot.DistFunc.dNonCentralChiSquare(nu, lambda_, x),
            )
# pNonCentralChiSquare
nuMin = 0.2
nuMax = 5.0
n1 = 5
lambdaMin = 0.2
lambdaMax = 5.0
n2 = 5
xMin = 0.1
xMax = 0.9
nX = 5
for i1 in range(n1):
    nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
    for i2 in range(n2):
        lambda_ = lambdaMin + (lambdaMax - lambdaMin) * i2 / (n2 - 1)
        for iX in range(nX):
            x = xMin + (xMax - xMin) * iX / (nX - 1)
            print(
                "pNonCentralChiSquare(",
                nu,
                ", ",
                lambda_,
                ",  %.12g" % x,
                ")=%.6g" % ot.DistFunc.pNonCentralChiSquare(nu, lambda_, x),
                ", complementary=%.6g"
                % ot.DistFunc.pNonCentralChiSquare(nu, lambda_, x, True),
            )
# rNonCentralChiSquare
nuMin = 0.2
nuMax = 5.0
n1 = 5
lambdaMin = 0.2
lambdaMax = 5.0
n2 = 5
nR = 5
for i1 in range(n1):
    nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
    for i2 in range(n2):
        lambda_ = lambdaMin + (lambdaMax - lambdaMin) * i2 / (n2 - 1)
        for iR in range(nR):
            print(
                "rNonCentralChiSquare(",
                nu,
                ", ",
                lambda_,
                ")=%.6g" % ot.DistFunc.rNonCentralChiSquare(nu, lambda_),
            )
