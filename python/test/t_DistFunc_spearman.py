#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Spearman related functions
lSize = [5, 10, 25, 50, 1000]
for size in lSize:
    for ties in [True, False]:
        for tail in [True, False]:
            for iRho in range(-10, 11):
                rho = 0.1 * iRho
                print(
                    "size=",
                    size,
                    " ties=",
                    ties,
                    "tail=",
                    tail,
                    " rho=%.6g" % rho,
                    " p=%.6g" % ot.DistFunc.pSpearmanCorrelation(size, rho, tail, ties),
                )
