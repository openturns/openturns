"""
Covariance model algebra: sums, scaling and structural nugget
==============================================================

This example shows the covariance algebra introduced with the
:class:`~openturns.experimental.SumCovarianceModel` and
:class:`~openturns.experimental.ScaledCovarianceModel` classes:

- how to add two covariance models and scale a model by a positive factor,
- how to set a nugget effect structurally, as a
  :class:`~openturns.DiracCovarianceModel` term of the sum, as an alternative
  to the transversal nugget factor (see :doc:`/auto_stochastic_processes/plot_covariance_model_nugget`
  for the nugget-factor formulation),
- how nested sums are flattened and identical terms folded automatically,
  as :class:`~openturns.LinearCombinationFunction` does for functions.

We illustrate each construction with sample paths of centered Gaussian
processes in 1D. Additive decompositions of a smooth trend, a quasi-periodic
term and white noise follow the compositional kernel search of
[duvenaud2013]_ and the automatic-statistician decomposition of
[lloyd2014]_.
"""

# %%
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

ot.Log.Show(ot.Log.NONE)
ot.RandomGenerator.SetSeed(0)

# sphinx_gallery_thumbnail_number = 2

# %%
# A smooth Matern structure with the legacy nugget switched off, plus an
# explicit white-noise leaf whose amplitude is the noise standard deviation.
smooth = ot.MaternModel([1.0], [2.0], 2.5)
smooth.setNuggetFactor(0.0)
noise = ot.DiracCovarianceModel(1, [0.5])
noise.setNuggetFactor(0.0)
total = smooth + noise
print("sum full parameter =", total.getFullParameter())
assert list(total.getFullParameter()) == list(
    otexp.SumCovarianceModel([smooth, noise]).getFullParameter()
)

# %%
# Centered Gaussian processes on a 1D mesh, with and without the noise term.
# The noisy paths are jagged while the smooth paths interpolate refinely.
mesh = ot.IntervalMesher([200]).build(ot.Interval(0.0, 10.0))
smooth_paths = ot.GaussianProcess(smooth, mesh).getSample(3).drawMarginal(0)
smooth_paths.setTitle("Smooth Matern paths")
noisy_paths = ot.GaussianProcess(total, mesh).getSample(3).drawMarginal(0)
noisy_paths.setTitle("Matern + Dirac noise paths")
grid = ot.GridLayout(2, 1)
grid.setGraph(0, 0, smooth_paths)
grid.setGraph(1, 0, noisy_paths)
view = otv.View(grid)

# %%
# Simplification, as with functions: nested sums flatten, identical atoms
# fold into a single scaled model (`m + m -> 2*m`, `2*m + 3*m -> 5*m`).
folded = (smooth * 2.0) + (smooth * 3.0)
print("folded full parameter =", folded.getFullParameter())
print("folded descriptions =", folded.getFullParameterDescription())
flat = smooth + noise + smooth
print("flattened size =", flat.getFullParameter().getSize())

# %%
# Additive decomposition in the automatic-statistician spirit: a
# long-range trend, a quasi-periodic term and white noise, summed on the
# same input space with one variance per structure.
trend = ot.SquaredExponential([5.0], [1.5])
trend.setNuggetFactor(0.0)
quasi_periodic = ot.ExponentiallyDampedCosineModel()
quasi_periodic.setNuggetFactor(0.0)
composite = trend + quasi_periodic + noise
print("composite full parameter =", composite.getFullParameter())
composite_paths = (
    ot.GaussianProcess(composite, mesh).getSample(3).drawMarginal(0)
)
composite_paths.setTitle("Trend + quasi-periodic + noise paths")
view = otv.View(composite_paths)

# %%
# Display all figures.
otv.View.ShowAll()
