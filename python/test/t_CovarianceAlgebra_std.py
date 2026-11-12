#! /usr/bin/env python

# End-to-end example of the covariance algebra, mirroring common Gaussian
# process idioms:
#   GPflow   : k = Matern52(variance) + White(variance)
#   GPyTorch : ScaleKernel(Matern) + GaussianLikelihood noise
#   OpenTURNS: smooth + Dirac, both first-class covariance models

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

sigma = 2.0
sigma_n = 0.5

# Smooth Matern structure with the legacy nugget switched off
smooth = ot.MaternModel([1.0], [sigma], 2.5)
smooth.setNuggetFactor(0.0)

# Explicit white-noise leaf: amplitude carries the noise standard deviation
noise = ot.DiracCovarianceModel(1, [sigma_n])
noise.setNuggetFactor(0.0)

# Sum and positive scaling with Function-like simplification
total = smooth + noise
explicit = otexp.LinearCombinationCovarianceModel([smooth, noise], [1.0, 1.0])
ott.assert_almost_equal(total.getFullParameter(), explicit.getFullParameter())
prior = 2.0 * smooth + noise
print("total =", total)
print("full parameter =", total.getFullParameter())

# Zero lag carries the noise variance, off-diagonal lags match the smooth part
ott.assert_almost_equal(total.computeAsScalar([0.0], [0.0]), sigma**2 + sigma_n**2)
ott.assert_almost_equal(
    total.computeAsScalar([0.0], [1.0]), smooth.computeAsScalar([0.0], [1.0])
)
ott.assert_almost_equal(
    total.computeAsScalar([0.0], [2.0]), smooth.computeAsScalar([0.0], [2.0])
)

# Equivalence with the legacy zero-lag factor tau = sigma_n^2 / sigma^2
legacy = ot.MaternModel([1.0], [sigma], 2.5)
legacy.setNuggetFactor(sigma_n**2 / sigma**2)
ott.assert_almost_equal(
    total.computeAsScalar([0.0], [0.0]), legacy.computeAsScalar([0.0], [0.0])
)
ott.assert_almost_equal(
    total.computeAsScalar([0.0], [1.0]), legacy.computeAsScalar([0.0], [1.0])
)

# Discretized Gram matrix: smooth block plus scaled identity
grid = ot.RegularGrid(0.0, 1.0, 3)
gram = total.discretize(grid)
expected = smooth.discretize(grid)
for k in range(3):
    expected[k, k] += sigma_n**2
ott.assert_almost_equal(gram, expected)
print("gram =\n", gram)

# Scaling composes: 2*K + K folds to a single variance weight
folded = (smooth * 2.0) + smooth
ott.assert_almost_equal(
    folded.computeAsScalar([0.0], [1.0]), 3.0 * smooth.computeAsScalar([0.0], [1.0])
)

# The coefficients are first in the full parameter and stay fittable, while
# the generic amplitude and nugget of the normalized atoms are frozen.
active_desc = [total.getFullParameterDescription()[i] for i in total.getActiveParameter()]
assert "coefficient_0" in active_desc
assert "coefficient_1" in active_desc
assert "atom_0_nuggetFactor" not in active_desc
assert "atom_1_nuggetFactor" not in active_desc
print("active =", active_desc)
