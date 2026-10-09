%feature("docstring") OT::ChristoffelSubsampleExperiment
R"RAW(Christoffel subsample experiment.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Given an orthonormal basis of :math:`L^2(D, \mu)` and a target size
:math:`n`, the experiment works with the first :math:`m` basis functions,
where :math:`m = n - k` is the largest dimension supplied by the basis:
:math:`k = 0` with an inexhaustible basis, and :math:`k = 1, 2, \dots` is
tried until the basis supplies :math:`m` functions. It draws a pool of points
from the :class:`~openturns.experimental.ChristoffelDistribution` associated to
these :math:`m` functions, sized by
``ChristoffelSubsampleExperiment-PoolOversamplingFactor`` times :math:`n`,
then greedily thins the pool down to :math:`n` points. The default ``Removal``
method drops pool points while keeping the largest smallest Gramian eigenvalue
and no reweighting; the ``Barrier`` method is a forward barrier greedy selection
with reweighting, clamped strictly inside the current spectrum so rank-deficient
steps stay well-defined. Each kept point :math:`x_i` carries a weight proportional
to :math:`m / k_m(x_i)`, the density ratio of the reference measure
over the Christoffel distribution, times the barrier weight for the
``Barrier`` method.

The sizing follows the budget theorem: from :math:`\gamma m \log m`
Christoffel draws with :math:`\gamma \approx 9.242` (Theorem 2.3 of Cohen
and Dolbeault [cohendolbeault2020]_) one extracts :math:`m + k` points
with a good approximation; the pool above is the practical pool, used at
the price of weaker guarantees.

Available constructors:
    ChristoffelSubsampleExperiment(*basis, size*)

Parameters
----------
basis : :class:`~openturns.OrthogonalBasis`
    Orthonormal basis defining the approximation space. Its embedded
    measure :math:`\mu` must be continuous.
size : positive int
    Number :math:`n` of points of the experiment.

See Also
--------
openturns.WeightedExperiment
openturns.experimental.ChristoffelDistribution

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``ChristoffelSubsampleExperiment-PoolOversamplingFactor`` (``Scalar``, default: ``2.0``): candidate pool size as a multiple of the target size.
- ``ChristoffelSubsampleExperiment-FrameTolerance`` (``Scalar``, default: ``0.5``): half-width of the accepted frame eigenvalue band around 1.
- ``ChristoffelSubsampleExperiment-ThinningMethod`` (``String``, default: ``Removal``): greedy thinning method. Possible values: ``Barrier``, ``Removal``.
- ``ChristoffelSubsampleExperiment-BarrierStep`` (``Scalar``, default: ``1.0``): barrier advance per selection step of the ``Barrier`` method.
- ``ChristoffelSubsampleExperiment-BarrierRegularization`` (``Scalar``, default: ``1e-8``): safety gap keeping the barriers strictly inside the spectrum.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> factory = ot.OrthogonalProductPolynomialFactory([ot.Uniform(-1.0, 1.0)])
>>> basis = ot.OrthogonalBasis(factory)
>>> experiment = otexp.ChristoffelSubsampleExperiment(basis, 60)
>>> print(experiment.getSpaceDimension())
60
>>> sample, weights = experiment.generateWithWeights()
>>> print(len(sample), len(weights))
60 60)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::getOrthogonalBasis
"Accessor to the orthogonal basis.

Returns
-------
basis : :class:`~openturns.OrthogonalBasis`
    Orthogonal basis defining the approximation space."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::setOrthogonalBasis
R"RAW(Accessor to the orthogonal basis.

Parameters
----------
basis : :class:`~openturns.OrthogonalBasis`
    Orthogonal basis defining the approximation space. Its embedded
    measure :math:`\mu` must be continuous.

Notes
-----
The space dimension and the Christoffel distribution are deduced
again from the new basis and the current size.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::getSpaceDimension
"Accessor to the space dimension deduced from the target size.

Returns
-------
spaceDimension : positive int
    Number of leading basis functions used."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::generateWithWeights
"Generate the weighted design.

Parameters
----------
poolSample : 2-d sequence of float, optional
    Existing points prepended to the candidate pool, e.g. already
    evaluated points to recycle. Fresh Christoffel draws top the
    pool up to the oversampling size.

Returns
-------
sample : :class:`~openturns.Sample`
    The thinned design of `size` points.
weights : :class:`~openturns.Point`
    The associated weights, recomputed at the kept points."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::computeDesignEigenvalues
"Eigenvalues of the weighted design Gramian.

Parameters
----------
sample : 2-d sequence of float
    Design points.
weights : sequence of float
    Weight of each design point.

Returns
-------
eigenvalues : :class:`~openturns.Point`
    Eigenvalues of the mean over the design of weight times outer
    product, the frame stability certificate."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::DeduceSpaceDimension
R"RAW(Deduce the space dimension from a target size.

With an inexhaustible basis the dimension is the target size
(:math:`k = 0` in :math:`m = n - k`); the constructors try
:math:`k = 1, 2, \dots` until the basis supplies :math:`m` functions.

Parameters
----------
size : positive int
    Target number of points.

Returns
-------
spaceDimension : positive int
    The target size itself.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::ComputeGramian
"Gramian of a subset of design points.

Parameters
----------
features : 2-d sequence of float
    Basis function values, one row per pool point.
weights : sequence of float
    Weight of each pool point.
kept : sequence of int
    Indices of the points kept in the subset.

Returns
-------
gramian : :class:`~openturns.SymmetricMatrix`
    Mean over the kept points of weight times outer product."
