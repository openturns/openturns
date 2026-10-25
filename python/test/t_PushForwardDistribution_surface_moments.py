#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# The deterministic moments of a nonlinear scalar map on R^3 are obtained by
# iterated quadrature over the input search box. The covariance needs a second
# quadrature over the same box, so this test is kept separate from
# t_PushForwardDistribution_std.py to keep each coverage test short.
surfaceCoArea = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x0^2 + x1^2 + x2^2"]),
    ot.Normal(3),
)
ott.assert_almost_equal(surfaceCoArea.getMean(), [3.0], 1e-8, 1e-12)
ott.assert_almost_equal(
    surfaceCoArea.getCovariance(), ot.CovarianceMatrix(1, [6.0]), 1e-8, 1e-12
)
