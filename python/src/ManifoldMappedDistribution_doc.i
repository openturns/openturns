%feature("docstring") OT::ManifoldMappedDistribution
R"RAW(Manifold mapped distribution.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The distribution on a Riemannian manifold obtained by pushing forward a
background Euclidean distribution of dimension equal to the intrinsic
dimension of the manifold through the exponential map at a chart center:
a background realization :math:`x` is mapped to
:math:`\exp(c, \sum_k x_k B_k)` where the :math:`B_k` form the tangent
basis of the manifold at the chart center :math:`c`.

The density with respect to the Riemannian volume follows from the
change of variables formula through the logarithmic map, corrected by
the volume density of the exponential map. At the cut locus of the chart
center the density is singular and is not evaluated.

Parameters
----------
background : :class:`~openturns.Distribution`, :math:`m`-dimensional
    The background Euclidean distribution, where :math:`m` is the
    intrinsic dimension of the manifold.
manifold : :class:`~openturns.experimental.Manifold`
    The manifold supporting the distribution.
chartCenter : sequence of float
    Chart center on the manifold.

See also
--------
Manifold, PushForwardDistribution

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``ManifoldMappedDistribution-SampleSize``: number of Monte-Carlo
  realizations used to estimate the range, the mean and the covariance,
  default value ``10000``.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> manifold = otexp.Manifold(otexp.SphereManifold(3))
>>> background = ot.Normal(2)
>>> chartCenter = [0.0, 0.0, 1.0]
>>> distribution = otexp.ManifoldMappedDistribution(background, manifold, chartCenter)
>>> print(distribution.getDimension())
3
>>> print(distribution.computePDF(chartCenter))
0.1591549...
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldMappedDistribution::getBackground
R"RAW(Accessor to the background distribution.

Returns
-------
background : :class:`~openturns.Distribution`
    The background Euclidean distribution.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldMappedDistribution::setBackground
R"RAW(Accessor to the background distribution.

Parameters
----------
background : :class:`~openturns.Distribution`, :math:`m`-dimensional
    The background Euclidean distribution.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldMappedDistribution::getManifold
R"RAW(Accessor to the manifold.

Returns
-------
manifold : :class:`~openturns.experimental.Manifold`
    The manifold supporting the distribution.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldMappedDistribution::setManifold
R"RAW(Accessor to the manifold.

Parameters
----------
manifold : :class:`~openturns.experimental.Manifold`
    The manifold supporting the distribution.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldMappedDistribution::getChartCenter
R"RAW(Accessor to the chart center.

Returns
-------
chartCenter : :class:`~openturns.Point`
    Chart center on the manifold.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldMappedDistribution::setChartCenter
R"RAW(Accessor to the chart center.

Parameters
----------
chartCenter : sequence of float
    Chart center on the manifold.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldMappedDistribution::getTangentBasis
R"RAW(Accessor to the tangent basis at the chart center.

Returns
-------
basis : :class:`~openturns.Sample`
    Orthonormal ambient vectors spanning the tangent space.)RAW"
