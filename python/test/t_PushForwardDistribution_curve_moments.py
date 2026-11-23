#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# The deterministic moments of a nonlinear map from R^3 to R^2 use iterated
# quadrature over the input search box. This is kept separate from
# t_PushForwardDistribution_std.py to keep each coverage test short.
# The integrand is smooth, so a higher-order rule with fewer subintervals
# reaches the structural truncation floor of the finite search box (the same
# values as with the default settings) at a fraction of the cost.
ot.ResourceMap.SetAsUnsignedInteger("IteratedQuadrature-MaximumSubIntervals", 8)
ot.ResourceMap.SetAsScalar("IteratedQuadrature-MaximumError", 1e-9)
ot.ResourceMap.SetAsString("IteratedQuadrature-Rule", "G11K23")
normalCurve = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x0^2 + x1^2", "x2"]),
    ot.Normal(3),
)
ott.assert_almost_equal(normalCurve.getMean(), [2.0, 0.0], 1e-8, 1e-9)
ott.assert_almost_equal(
    normalCurve.getCovariance(),
    ot.CovarianceMatrix(2, [4.0, 0.0, 0.0, 1.0]),
    1e-8,
    1e-9,
)
ot.ResourceMap.Reset()
