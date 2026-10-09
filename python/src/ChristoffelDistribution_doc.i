%feature("docstring") OT::ChristoffelDistribution
R"RAW(Christoffel distribution.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The Christoffel distribution associated to the first :math:`m` functions
:math:`(L_1, \dots, L_m)` of an orthonormal basis of :math:`L^2(D, \mu)`
is the probability measure :math:`\sigma` defined by

.. math::
    \mathrm{d}\sigma(x) = \frac{k_m(x)}{m} \, \mathrm{d}\mu(x),
    \quad k_m(x) = \sum_{j=1}^m L_j(x)^2,
    \quad x \in D,

where :math:`k_m` is the Christoffel function and :math:`\mu` is the
reference measure embedded in the basis. When :math:`\mu` is absolutely
continuous with density :math:`p_\mu`, the density of :math:`\sigma` is

.. math::
    p(x) = p_\mu(x) \, \frac{k_m(x)}{m}.

Orthonormality with respect to :math:`\mu` is a precondition, and the
class does not check it. It is what makes :math:`k_m / m` a density.
The stability factor and the sampling acceptance rates rely on it.
A cheap numerical check is the trace identity
:math:`\int k_m \, \mathrm{d}\mu = m`, ie the empirical mean of
:meth:`computeChristoffelFunction` over a sample of :math:`\mu`.
For a correlated :math:`\mu`, compose tensor-orthonormal functions with
a whitening map of :math:`\mu`. For a multivariate normal, use
:math:`x \mapsto L^{-1} x` with :math:`L` the Cholesky factor of the
correlation. This construction is needed because
:class:`~openturns.experimental.FiniteOrthogonalFunctionFactory` stores
the given functions as given and only checks their dimensions.

When the reference measure factorizes, sampling uses the tensor
sequential path. Otherwise the class compares two samplers at the first
draw, once :meth:`computeKn` is known, and keeps the one with the larger
measured acceptance rate. The candidates are the internal
ratio-of-uniforms sampler and rejection from the reference under the
stability factor envelope.

Available constructors:
    ChristoffelDistribution(*basis, size*)

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
- ``ChristoffelDistribution-KnSafetyFactor`` (``Scalar``, default: ``1.1``): multiplicative safety margin of the stability factor estimate.
- ``ChristoffelDistribution-SliceGridSize`` (``UnsignedInteger``, default: ``1000``): grid size of the univariate slice maxima in sequential conditional sampling.

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
    times the safety margin. The value is cached until the basis or the
    size changes.

Notes
-----
With ``ChristoffelDistribution-ExactKn`` enabled (default) the estimate is
exact up to the coverage of the multi-start optimization: a local maximum
may be missed, hence the safety margin
(``ChristoffelDistribution-KnSafetyFactor``, default 1.1). If the
optimization fails, the plain Monte-Carlo maximum over the candidate pool
is returned instead, and the samplers refresh the envelope on the fly
whenever a proposal exceeds it."

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
