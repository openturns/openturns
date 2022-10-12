#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

distParams = []
distParams.append(ot.ArcsineMuSigma(8.4, 2.25))
distParams.append(ot.BetaMuSigma(0.2, 0.6, -1, 2))
distParams.append(ot.GammaMuSigma(1.5, 2.5, -0.5))
distParams.append(ot.GumbelLambdaGamma(0.6, 6.0))
distParams.append(ot.GumbelMuSigma(1.5, 1.3))
distParams.append(ot.LogNormalMuErrorFactor(0.63, 1.5, -0.5))
distParams.append(ot.LogNormalMuSigma(0.63, 3.3, -0.5))
distParams.append(ot.LogNormalMuSigmaOverMu(0.63, 5.24, -0.5))
distParams.append(ot.UniformMuSigma(8.4, 2.25))
distParams.append(ot.WeibullMaxMuSigma(1.3, 1.23, 3.1))
distParams.append(ot.WeibullMinMuSigma(1.3, 1.23, -0.5))

for distParam in distParams:

    print("Distribution Parameters ", repr(distParam))
    print("Distribution Parameters ", distParam)

    p = distParam.getValues()
    desc = distParam.getDescription()
    print("p=", p, desc)
    p_native = distParam.evaluate()
    print("native=", p_native)
    p_roundtrip = distParam.inverse(p_native)
    print("roundtrip=", p_roundtrip)
    ott.assert_almost_equal(p_roundtrip, p)
    dist = distParam.getDistribution()
    print("built dist=", dist)
    assert dist.getParameter() == p_native

    # derivative of the native parameters with regards the parameters of the
    # distribution
    grad = distParam.gradient()
    print(grad)

    # by the finite difference technique
    eps = 1e-5
    dim = len(p)
    grad_fd = ot.SquareMatrix(ot.IdentityMatrix(dim))

    for i in range(dim):
        for j in range(dim):
            xp = list(p)
            xp[i] += eps
            xm = list(p)
            xm[i] -= eps
            grad_fd[i, j] = 0.5 * (distParam(xp)[j] - distParam(xm)[j]) / eps
    print(grad_fd)
    ott.assert_almost_equal(grad, grad_fd)
