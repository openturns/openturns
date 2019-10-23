
import openturns.shims as ot
import openturns.testing as ott

assert ot.Arcsine() is not None

print(ot.Beta(2.0, 4.0, 0.0, 1.0))
ott.assert_almost_equal(
    ot.Beta(2.0, 4.0, 0.0, 1.0).getParameter(), [2.0, 2.0, 0.0, 1.0])

print(ot.Frechet(6.0, 1.5))
ott.assert_almost_equal(ot.Frechet(6.0, 1.5).getParameter(), [1.5, 6.0, 0.0])

print(ot.Gumbel(558.0, 1013.0))
ott.assert_almost_equal(
    ot.Gumbel(558.0, 1013.0).getStandardDeviation(), [0.00229848])

print(ot.InverseGamma(2.0, 3.0))
ott.assert_almost_equal(ot.InverseGamma(2.0, 3.0).getParameter(), [3.0, 2.0])

print(ot.InverseNormal(2.0, 3.0))
ott.assert_almost_equal(ot.InverseNormal(2.0, 3.0).getParameter(), [3.0, 2.0])

print(ot.Laplace(2.0, 3.0))
ott.assert_almost_equal(ot.Laplace(2.0, 3.0).getParameter(), [3.0, 2.0])
