%feature("docstring") OT::ChristoffelSubsampleExperiment
R"RAW(Christoffel subsample experiment.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Given an orthonormal basis of :math:`L^2(D, \mu)` and a target size
:math:`n`, the experiment draws a pool of points from the
:class:`~openturns.experimental.ChristoffelDistribution` associated to the
first :math:`m` basis functions, where :math:`m` is deduced from

.. math::
    n = \gamma \, m \, \log m,

with :math:`\gamma = \left(3/2 \log(3/2) - 1/2\right)^{-1} = 9.242343873386666`
by default, the constant of Theorem 2.3 of Cohen-Dolbeault, then greedily thins
the pool down to
:math:`n` points. The default ``Removal`` method drops pool points while
keeping the largest smallest Gramian eigenvalue and no reweighting; the
``Barrier`` method is a forward barrier greedy selection with reweighting,
clamped strictly inside the current spectrum so rank-deficient steps stay
well-defined. Each kept point :math:`x_i` carries a weight proportional
to :math:`m / k_m(x_i)`, the density ratio of the reference measure
over the Christoffel distribution, times the barrier weight for the
``Barrier`` method.

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

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``ChristoffelSubsampleExperiment-Gamma`` (``Scalar``, default: ``9.242343873386666``): gamma constant :math:`\gamma` of the sizing rule. The value :math:`\gamma \approx 9.242` is the one in Theorem 2.3 of Cohen-Dolbeault; smaller values are used in practice at the price of weaker guarantees.
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
4
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

Parameters
----------
size : positive int
    Target number of points.

Returns
-------
spaceDimension : positive int
    Largest integer with gamma times spaceDimension times
    log(spaceDimension) not larger than size, where
    :math:`\gamma \approx 9.242`
    is the default of the ``ChristoffelSubsampleExperiment-Gamma`` key.)RAW"

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
