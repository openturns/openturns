#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

distParams = []
distParams.append(ot.ArcsineMuSigma(8.4, 2.25))
distParams.append(ot.BetaMuSigma(0.2, 0.6, -1, 2))
distParams.append(ot.GammaMuSigma(1.5, 2.5, -0.5))
distParams.append(ot.GumbelLambdaGamma(0.6, 6.0))
distParams.append(ot.GumbelMuSigma(1.5, 1.3))
distParams.append(ot.LogNormalMuErrorFactor(0.63, 1.5, -0.5))
distParams.append(ot.LogNormalMuSigma(0.63, 3.3, -0.5))
distParams.append(ot.LogNormalMuSigmaOverMu(0.63, 5.24, -0.5))
distParams.append(ot.WeibullMaxMuSigma(1.3, 1.23, 3.1))
distParams.append(ot.WeibullMinMuSigma(1.3, 1.23, -0.5))

for distParam in distParams:

    print('Distribution Parameters ', repr(distParam))
    print('Distribution Parameters ', distParam)

    non_native = distParam.getValues()
    desc = distParam.getDescription()
    print('non-native=', non_native, desc)
    native = distParam.evaluate()
    print('native=', native)
    non_native = distParam.inverse(native)
    print('non-native=', non_native)
    print('built dist=', distParam.getDistribution())

    # derivative of the native parameters with regards the parameters of the
    # distribution
    print(distParam.gradient())

    # by the finite difference technique
    eps = 1e-5
    dim = len(non_native)
    nativeParamGrad = ot.SquareMatrix(ot.IdentityMatrix(dim))

    for i in range(dim):
        for j in range(dim):
            xp = list(non_native)
            xp[i] += eps
            xm = list(non_native)
            xm[i] -= eps
            nativeParamGrad[i, j] = 0.5 * \
                (distParam(xp)[j] - distParam(xm)[j]) / eps

    print(nativeParamGrad)
