#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

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
            print("kFactor(%d, %.12g, %.12g)=%.6g" % (2 * n, p, alpha, kF))
mMin = 1
mMax = 3
for n in range(nMin, nMax):
    for m in range(mMin, mMax):
        for iP in range(nP + 1):
            p = pMin + (pMax - pMin) * iP / nP
            for iAlpha in range(nAlpha + 1):
                alpha = alphaMin + (alphaMax - alphaMin) * iAlpha / nAlpha
                kF = ot.DistFunc.kFactorPooled(2 * n, m, p, alpha)
                print(
                    "kFactorPooled(%d, %.12g, %.12g, %.12g)=%.6g"
                    % (2 * n, m, p, alpha, kF)
                )

# Non-positive RMax is rejected before bracketing
ot.ResourceMap.SetAsScalar("KFactor-RMax", 0.0)
try:
    with ott.assert_raises(TypeError):
        ot.DistFunc.kFactor(4, 0.5, 0.5)
finally:
    ot.ResourceMap.SetAsScalar("KFactor-RMax", 8.5)

# Non-finite RMax is rejected the same way
ot.ResourceMap.SetAsScalar("KFactor-RMax", float("inf"))
try:
    with ott.assert_raises(TypeError):
        ot.DistFunc.kFactor(4, 0.5, 0.5)
finally:
    ot.ResourceMap.SetAsScalar("KFactor-RMax", 8.5)
