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
# defaults
distParams.append(ot.ArcsineMuSigma())
distParams.append(ot.BetaMuSigma())
distParams.append(ot.GammaMuSigma())
distParams.append(ot.GumbelLambdaGamma())
distParams.append(ot.GumbelMuSigma())
distParams.append(ot.LogNormalMuErrorFactor())
distParams.append(ot.LogNormalMuSigma())
distParams.append(ot.LogNormalMuSigmaOverMu())
distParams.append(ot.UniformMuSigma())
distParams.append(ot.WeibullMaxMuSigma())
distParams.append(ot.WeibullMinMuSigma())

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

# DistributionParameters equality tests
assert ot.UniformMuSigma(8.4, 2.25) == ot.UniformMuSigma(8.4, 2.25), "UniformMuSigma == UniformMuSigma"
assert not (ot.UniformMuSigma(8.4, 2.25) == ot.GumbelMuSigma(1.5, 1.3)), "not UniformMuSigma == GumbelMuSigma"
assert not (ot.UniformMuSigma(8.4, 2.25) == ot.UniformMuSigma(8.0, 2.25)), "not UniformMuSigma == UniformMuSigma diff params"
assert ot.GumbelMuSigma(1.5, 1.3) != ot.WeibullMinMuSigma(1.3, 1.23, -0.5), "GumbelMuSigma != WeibullMinMuSigma"
assert not (ot.WeibullMaxMuSigma(1.3, 1.23, 3.1) != ot.WeibullMaxMuSigma(1.3, 1.23, 3.1)), "not WeibullMaxMuSigma != WeibullMaxMuSigma"
assert ot.ArcsineMuSigma(8.4, 2.25) == ot.ArcsineMuSigma(8.4, 2.25), "ArcsineMuSigma == ArcsineMuSigma"
assert not (ot.BetaMuSigma(0.2, 0.6, -1, 2) == ot.BetaMuSigma(0.3, 0.6, -1, 2)), "not BetaMuSigma == BetaMuSigma diff mu"
assert ot.GammaMuSigma(1.5, 2.5, -0.5) != ot.GumbelMuSigma(1.5, 1.3), "GammaMuSigma != GumbelMuSigma"
assert ot.LogNormalMuSigma(0.63, 3.3, -0.5) == ot.LogNormalMuSigma(0.63, 3.3, -0.5), "LogNormalMuSigma == LogNormalMuSigma"
assert ot.GumbelLambdaGamma(0.6, 6.0) != ot.LogNormalMuSigma(0.63, 3.3, -0.5), "GumbelLambdaGamma != LogNormalMuSigma"
assert ot.LogNormalMuSigmaOverMu(0.63, 5.24, -0.5) == ot.LogNormalMuSigmaOverMu(0.63, 5.24, -0.5), "LogNormalMuSigmaOverMu == LogNormalMuSigmaOverMu"
assert ot.LogNormalMuErrorFactor(0.63, 1.5, -0.5) != ot.LogNormalMuSigma(0.63, 3.3, -0.5), "LogNormalMuErrorFactor != LogNormalMuSigma"
assert ot.WeibullMinMuSigma(1.3, 1.23, -0.5) == ot.WeibullMinMuSigma(1.3, 1.23, -0.5), "WeibullMinMuSigma == WeibullMinMuSigma"

# DistributionParameters handle equality tests
p1 = ot.DistributionParameters(ot.UniformMuSigma(8.4, 2.25))
p2 = ot.DistributionParameters(ot.UniformMuSigma(8.4, 2.25))
p3 = ot.DistributionParameters(ot.GumbelMuSigma(1.5, 1.3))
assert p1 == p2, "DP UniformMuSigma == DP UniformMuSigma"
assert not (p1 == p3), "not DP UniformMuSigma == DP GumbelMuSigma"
assert p1 != p3, "DP UniformMuSigma != DP GumbelMuSigma"
assert not (p1 != p2), "not DP UniformMuSigma != DP UniformMuSigma"
