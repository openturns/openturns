%feature("docstring") OT::BasisOrthonormalizationAlgorithm
R"RAW(Basis orthonormalization algorithm.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class computes an orthonormal basis from the first :math:`s` elements of
an :class:`~openturns.OrthogonalBasis`, with respect to the basis measure
:math:`\mu`. The initial functions are ``basis.build(0)``, ...,
``basis.build(s - 1)`` and the inner product is

.. math::

    \langle f, g \rangle
    = \int f(\vect{x}) g(\vect{x}) \, \mu(\mathrm{d}\vect{x}),
    \qquad \mu = \mathtt{basis.getMeasure()}.

The initial functions do not need to be orthonormal with respect to
:math:`\mu`: the algorithm computes an orthonormal basis of the vector space
they span. An
:class:`~openturns.experimental.FiniteOrthogonalFunctionFactory` can be used
to expose an arbitrary collection of functions and its measure as an
:class:`~openturns.OrthogonalBasis`.

Two routes are available depending on the integration rule:

- experiment-derived rules expose their nodes: the basis comes from the QR
  factorization of the weighted design matrix, which preserves the
  conditioning of the design;
- opaque rules only provide integrals: the basis comes from the Cholesky
  factorization :math:`G = R^T R` of the Gram matrix, whose condition
  number is the square of the design's, hence less robust on
  ill-conditioned bases.

A weighted experiment wrapped into an
:class:`~openturns.ExperimentIntegration` algorithm must integrate against
the basis measure: for a continuous experiment, the experiment distribution
must be the basis measure and its instrumental distribution must contain the
support of the basis measure. An exception is raised otherwise, because a
different measure or a missing part of the support would silently produce
functions that are orthonormal for another measure. A discrete experiment
such as :class:`~openturns.FixedExperiment` is exempt: its distribution is
the empirical measure supported by the nodes, whose range is the convex hull
of the nodes, which may legitimately be smaller than the basis support. A
custom :class:`~openturns.IntegrationAlgorithm` integrates against the
Lebesgue measure, the density being accounted for internally.

The default integration rule is built from the basis measure: an independent
tensor Gauss product rule is tried first, then, when the standard space is
standard normal, a Gauss-Hermite product rule transported through the
inverse iso-probabilistic transformation, then a Gauss-Legendre rule on the
numerical range with the density folded into the weights. This makes a wide
range of measures usable without a manual integration rule. The integration
algorithm is not a constructor argument: the default rule is used unless
:meth:`setIntegrationAlgorithm` replaces it, so that the basis, its measure
and the rule are always set consistently.

Parameters
----------
basis : :class:`~openturns.OrthogonalBasis`
    Orthogonal basis whose first elements are orthonormalized against its
    measure.
size : int
    Number of first elements of the basis to orthonormalize. It must be
    positive.

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``BasisOrthonormalizationAlgorithm-DefaultDiscretization`` (``UnsignedInteger``, default: ``128``): requested number of 1D nodes in the Gauss product experiment.
- ``BasisOrthonormalizationAlgorithm-MaximumTensorNodes`` (``UnsignedInteger``, default: ``1000000``): maximum number of nodes of the tensor rule used to build the default integration. The marginal order is reduced when the full tensor product would exceed this budget, and the rule is rejected when the budget cannot even hold the order-2 tensor product.
- ``BasisOrthonormalizationAlgorithm-Epsilon`` (``Scalar``, default: ``1.0e-11``): relative threshold for zeroing coefficients and detecting linearly dependent functions.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> dim = 2
>>> distribution = ot.JointDistribution(
...     [ot.Uniform(-1.0, 1.0)] * dim, ot.FrankCopula(4.0))
>>> legendreBasis = ot.OrthogonalProductPolynomialFactory(
...     [ot.LegendreFactory()] * dim)
>>> functions = [legendreBasis.build(i) for i in range(6)]
>>> initialBasis = ot.OrthogonalBasis(
...     otexp.FiniteOrthogonalFunctionFactory(functions, distribution))
>>> algo = otexp.BasisOrthonormalizationAlgorithm(initialBasis, len(functions))
>>> algo.run()
>>> len(algo.getOrthonormalFunctions())
6
)RAW"

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::run
"Compute the orthonormal basis."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::setIntegrationAlgorithm
"Accessor to the integration algorithm.

A rule built from a weighted experiment (e.g.
:class:`~openturns.ExperimentIntegration`) exposes its nodes and is used
through a QR factorization. For a continuous experiment, the experiment
distribution must be the basis measure, and its instrumental distribution
must contain the support of the basis measure; otherwise an exception is
raised. A discrete experiment such as :class:`~openturns.FixedExperiment` is
exempt because its distribution is the empirical measure supported by its
nodes. Any other rule provides only integrals and is used through a Cholesky
factorization of the Gram matrix; it integrates against the Lebesgue measure,
the density being accounted for internally.

Parameters
----------
integrationAlgorithm : :class:`~openturns.IntegrationAlgorithm`
    Integration algorithm."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getIntegrationAlgorithm
"Accessor to the integration algorithm.

Returns
-------
integrationAlgorithm : :class:`~openturns.IntegrationAlgorithm`
    Integration algorithm."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getOrthogonalBasis
"Accessor to the orthogonal basis.

Returns
-------
basis : :class:`~openturns.OrthogonalBasis`
    Orthogonal basis whose first elements are orthonormalized."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getSize
"Accessor to the number of orthonormalized basis elements.

Returns
-------
size : int
    Number of first elements of the orthogonal basis which are
    orthonormalized."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getMeasure
"Accessor to the measure.

Returns
-------
measure : :class:`~openturns.Distribution`
    Measure of the orthogonal basis, with respect to which the functions are
    orthonormalized."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getOrthonormalFunctions
"Accessor to the orthonormal functions.

The orthonormal basis is computed on first access if it has not been
computed yet.

Returns
-------
functions : sequence of :class:`~openturns.Function`
    Orthonormal functions, ordered as the basis elements."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getCoefficients
"Accessor to the coefficient matrix.

The orthonormal basis is computed on first access if it has not been
computed yet. Column :math:`j` of the matrix contains the coefficients of
the :math:`j`-th orthonormal function in the first elements of the
orthogonal basis.

Returns
-------
coefficients : :class:`~openturns.SquareMatrix`
    Coefficients of the orthonormal functions in the orthogonal basis."
