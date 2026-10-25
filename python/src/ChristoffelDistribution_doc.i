%feature("docstring") OT::ChristoffelDistribution
R"RAW(Christoffel distribution.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The Christoffel distribution associated to the first :math:`m` functions
:math:`(\varphi_1, \dots, \varphi_m)` of an orthonormal basis of
:math:`L^2(\cD, \mu)` is the probability measure :math:`\sigma_m` defined
by

.. math::
    \di{\sigma_m(\vect{x})} = \frac{k_m(\vect{x})}{m} \di{\mu(\vect{x})},
    \qquad
    k_m(\vect{x}) = \sum_{j=1}^m \varphi_j(\vect{x})^2,
    \qquad \vect{x} \in \cD,

where :math:`k_m` is the Christoffel function and :math:`\mu` is the
reference measure embedded in the basis. When :math:`\mu` has density
:math:`p_\mu`, the density of :math:`\sigma_m` is

.. math::
    p_{\sigma_m}(\vect{x}) = p_\mu(\vect{x}) \frac{k_m(\vect{x})}{m}.

The density is proportional to the pointwise energy of the approximation
space: samples concentrate where the basis functions are large. The
measure depends only on the space spanned by the functions, not on the
particular orthonormal basis. See :any:`christoffel_distribution` for the
definition and the optimality properties of this measure, and
:any:`christoffel_sampling` for the sampling theory.

Orthonormality with respect to :math:`\mu` is a precondition, and the
class does not check it. It is what makes :math:`k_m / m` a density.
The stability factor and the sampling acceptance rates rely on it.
A cheap numerical check is the trace identity
:math:`\int_\cD k_m \di{\mu} = m`, ie the empirical mean of
:meth:`computeChristoffelFunction` over a sample of :math:`\mu`.
For a correlated :math:`\mu`, compose tensor-orthonormal functions with
a whitening map of :math:`\mu`. For a multivariate normal, use
:math:`\vect{x} \mapsto \mat{L}^{-1} \vect{x}` with :math:`\mat{L}` the
Cholesky factor of the correlation. This construction is needed because
:class:`~openturns.experimental.FiniteOrthogonalFunctionFactory` stores
the given functions as given and only checks their dimensions.

Rejection sampling is exact only when the numerical envelope bounds the
density ratio. When the basis is a tensor product and the reference
measure factorizes, the class uses sequential conditional rejection on the
tensor slices, with uniform proposals for bounded marginals and Cauchy
proposals for unbounded ones. Otherwise it compares the internal
ratio-of-uniforms sampler with rejection from the reference under the
stability factor envelope, and keeps the branch with the larger measured
acceptance rate; the ratio-of-uniforms branch is set up on the numerical
range. The envelope is scanned on a grid uniform in the proposal CDF and
multiplied by a safety factor: a proposal that exceeds it is discarded after
raising it, so a draw is never accepted above the envelope, but the scan is a
numerical estimate and not a proof of the global supremum. Raising the
envelope after a later exceedance cannot correct a draw already accepted
under an earlier, too low envelope. See
:any:`christoffel_sampling` for the algorithms and their numerical safeguards.

The interval returned by :meth:`getRange` is a numerical range, not
necessarily the support. :meth:`computePDF` returns zero when the reference
density evaluates to zero; for an unbounded reference it may evaluate points
beyond that numerical range when their density is positive.
The tensor sequential sampler likewise does not truncate an unbounded
marginal to that range. Conversely, a reference density that underflows to
zero makes the PDF zero even when the reference log-density is finite; use
:meth:`computeLogPDF` when tail values are needed.

Parameters
----------
basis : :class:`~openturns.OrthogonalBasis`
    Orthonormal basis defining the approximation space. Its embedded
    measure :math:`\mu` must be continuous.
size : positive int
    Dimension :math:`m` of the approximation space, ie the number of
    leading basis functions used.

See Also
--------
openturns.Distribution

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``ChristoffelDistribution-OptimizationAlgorithm`` (``String``, default: ``TNC``): optimization algorithm of the exact stability factor search and of the internal ratio-of-uniforms sampler. ``TNC`` uses the gradient of the log-objective (three-term recurrence for tensor polynomial bases, centered differences otherwise) and converges with an order of magnitude fewer evaluations than the gradient-free ``Cobyla``, which remains available through this key.
- ``ChristoffelDistribution-RatioUniformCandidateNumber`` (``UnsignedInteger``, default: ``10000``): number of candidate points of the internal ratio-of-uniforms sampler.
- ``ChristoffelDistribution-RatioUniformMaxDimension`` (``UnsignedInteger``, default: ``5``): input dimension above which the ratio-of-uniforms setup is not attempted and sampling uses rejection. Within this guard, whenever the setup succeeds, sampling picks the branch with the larger measured acceptance rate: rejection accepts at exactly ``size / kn``, and the ratio-of-uniforms sampler reports its own rate. A setup failure at any dimension falls back to rejection.
- ``ChristoffelDistribution-KnSamplingSize`` (``UnsignedInteger``, default: ``100000``): size of the candidate pool of the stability factor search.
- ``ChristoffelDistribution-ExactKn`` (``Bool``, default: ``True``): refine the stability factor by a multi-start bounded maximization instead of keeping the Monte-Carlo maximum.
- ``ChristoffelDistribution-KnMaximumMultiStart`` (``UnsignedInteger``, default: ``16``): number of best candidates used as starting points of that maximization.
- ``ChristoffelDistribution-KnSafetyFactor`` (``Scalar``, default: ``1.1``): multiplicative safety margin of the stability factor estimate. It must be positive and finite; the samplers throw if it is not.
- ``ChristoffelDistribution-SliceGridSize`` (``UnsignedInteger``, default: ``1000``): grid size of the rejection envelope scan in sequential conditional sampling, uniform in the proposal CDF.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> factory = ot.OrthogonalProductPolynomialFactory([ot.Uniform(-1.0, 1.0)])
>>> basis = ot.OrthogonalBasis(factory)
>>> distribution = otexp.ChristoffelDistribution(basis, 3)
>>> print(distribution.getSize())
3
>>> print(f"{distribution.computePDF([0.5]):.6f}")
0.304688
>>> print(distribution.computePDF([0.5]) == distribution.computePDF([-0.5]))
True)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::getOrthogonalBasis
"Accessor to the orthogonal basis.

Returns
-------
basis : :class:`~openturns.OrthogonalBasis`
    Orthogonal basis defining the approximation space."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::setOrthogonalBasis
"Accessor to the orthogonal basis.

Parameters
----------
basis : :class:`~openturns.OrthogonalBasis`
    Orthogonal basis defining the approximation space.
size : positive int
    Number of leading basis functions used.

Notes
-----
The Christoffel function, the numerical range and the samplers are
rebuilt from the new basis."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::getSize
"Accessor to the space dimension.

Returns
-------
size : positive int
    Number of leading basis functions used."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::getMeasure
"Accessor to the reference measure embedded in the basis.

Returns
-------
measure : :class:`~openturns.Distribution`
    Reference measure with respect to which the basis is orthonormal."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::getBasis
"Accessor to the finite basis of leading functions.

Returns
-------
basis : :class:`~openturns.Basis`
    First functions of the orthogonal basis."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::computeChristoffelFunction
"Evaluate the Christoffel function.

Parameters
----------
point : sequence of float or 2-d sequence of float
    Point or sample where the Christoffel function is evaluated.

Returns
-------
values : float or :class:`~openturns.Sample`
    Sum of the squared basis functions at the given point or sample."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::computeLogChristoffelFunction
"Stable natural logarithm of the Christoffel function.

Parameters
----------
point : sequence of float or 2-d sequence of float
    Point or sample where the Christoffel function is evaluated.

Returns
-------
values : float or :class:`~openturns.Sample`
    Natural logarithm of the sum of the squared basis functions, computed
    by factoring out the largest square. Unlike the logarithm of
    :meth:`computeChristoffelFunction` it neither overflows nor underflows when
    single basis functions are extreme, so it is the recommended entry
    point in the tails."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::computeKn
"Stability factor estimate, ie approximate supremum of the Christoffel function.

Returns
-------
kn : float
    Maximum of the Christoffel function over a candidate pool of size
    ``ChristoffelDistribution-KnSamplingSize`` built from a
    low-discrepancy sequence pushed through the inverse Rosenblatt
    transform of the reference measure, refined by a multi-start bounded
    maximization when ``ChristoffelDistribution-ExactKn`` is enabled,
    times the safety margin. The value is cached until the basis, the
    size, or one of the keys that enter the estimate changes
    (``KnSamplingSize``, ``ExactKn``, ``KnMaximumMultiStart``,
    ``KnSafetyFactor``, ``OptimizationAlgorithm``). A non-positive or
    non-finite safety factor is rejected.

Notes
-----
With ``ChristoffelDistribution-ExactKn`` enabled (default) the estimate is
reliable up to the coverage of the multi-start optimization: a local maximum
may be missed, hence the safety margin
(``ChristoffelDistribution-KnSafetyFactor``, default 1.1). If the
optimization fails, the plain Monte-Carlo maximum over the candidate pool
is returned instead, and the samplers raise their envelope and discard the
proposal whenever one exceeds it."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::computeConditionalPDF
"Conditional PDF of a component given the previous ones.

Parameters
----------
x : float
    Value of the conditioned component.
y : sequence of float
    Values of the conditioning components.

Returns
-------
value : float
    Closed slice form in the tensor-product case, generic
    implementation otherwise."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::computeConditionalCDF
"Conditional CDF of a component given the previous ones.

Parameters
----------
x : float
    Value of the conditioned component.
y : sequence of float
    Values of the conditioning components.

Returns
-------
value : float
    Slice quadrature in the tensor-product case, generic
    implementation otherwise."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::computeSequentialConditionalPDF
"Conditional PDFs of each component given the previous ones.

Parameters
----------
x : sequence of float
    Point where the sequential conditionals are evaluated.

Returns
-------
values : :class:`~openturns.Point`
    Fan-out to the scalar conditional PDFs in the tensor-product
    case, generic implementation otherwise."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelDistribution::computeSequentialConditionalCDF
"Conditional CDFs of each component given the previous ones.

Parameters
----------
x : sequence of float
    Point where the sequential conditionals are evaluated.

Returns
-------
values : :class:`~openturns.Point`
    Fan-out to the scalar conditional CDFs in the tensor-product
    case, generic implementation otherwise."
