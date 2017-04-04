#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

nMin = 2
nMax = 4
pMin = 0.1
pMax = 0.9
nP = 2
alphaMin = 0.1
alphaMax = 0.9
nAlpha = 2
for n in range(nMin, nMax):
    for iP in range(nP + 1):
        p = pMin + (pMax - pMin) * iP / nP
        for iAlpha in range(nAlpha + 1):
            alpha = alphaMin + (alphaMax - alphaMin) * iAlpha / nAlpha
            kF = ot.DistFunc.kFactor(2 * n, p, alpha)
            print('kFactor(%d, %.12g, %.12g)=%.6g' % (2 * n, p, alpha, kF))
mMin = 1
mMax = 3
for n in range(nMin, nMax):
    for m in range(mMin, mMax):
        for iP in range(nP + 1):
            p = pMin + (pMax - pMin) * iP / nP
            for iAlpha in range(nAlpha + 1):
                alpha = alphaMin + (alphaMax - alphaMin) * iAlpha / nAlpha
                kF = ot.DistFunc.kFactorPooled(2 * n, m, p, alpha)
                print('kFactorPooled(%d, %.12g, %.12g, %.12g)=%.6g' %
                      (2 * n, m, p, alpha, kF))
