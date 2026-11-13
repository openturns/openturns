%feature("docstring") OT::LineSamplingQuantile
R"RAW(Line-sampling quantile estimator.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    LineSamplingQuantile(*output, alpha=0.95, beta=0.95*)

The algorithm estimates the quantile of a scalar output
:math:`Y=\model(\inputRV)` by a deterministic bisection on the
line-sampling probability.  An important direction :math:`\bde` is fixed
before the root-finding loop:

- the value set by :meth:`setInitialAlpha` when available;
- otherwise the design point returned by FORM at a pilot threshold;
- otherwise the pilot standard point with the largest norm.

The orthogonal offsets are drawn once and reused for every candidate
threshold.  Along each line :math:`\inputReal_i+r\bde`, the probability
that the limit state is exceeded is a normal tail probability, and the
line estimate is the average of the per-line contributions.  The outer
root is bracketed geometrically around a pilot quantile and bisected to
:meth:`getRootTolerance`.

The line estimator and its confidence interval are described in
:any:`quantile_theory_directional`.  The important direction is never
updated across thresholds, because doing so would break the
common-random-number coupling that makes the outer objective well
defined.  The interval follows from the delta method applied to the
per-line contributions.  Its coverage is asymptotic, and it is labelled
``COVERAGE_NONE`` when the derivative is not valid or when the root did
not converge.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
alpha : float, optional
    Quantile level, in :math:`(0, 1)`.  Default is 0.95.
beta : float, optional
    Confidence level of the interval, in :math:`(0, 1)`.  Default is
    0.95.

Notes
-----
The class requires an output random vector that can be composed into a
threshold event.  A usual scalar random vector is wrapped automatically.
The sample size is the number of lines and must be at least two for the
delta-method interval.

The result returned by :meth:`run` stores the important direction, the
line count, the conditional-probability sample, the root bracket, the
root iteration count and the number of probability evaluations.

The following :class:`~openturns.ResourceMap` keys are used:

- ``LineSampling-DefaultSearchOppositeDirection`` (``Bool``, default: ``True``): whether the opposite direction is searched by default.
- ``QuantileRootSolver-DefaultTolerance`` (``Scalar``, default: ``1.0e-4``): default relative root-finding tolerance.
- ``QuantileRootSolver-DefaultMaximumIteration`` (``UnsignedInteger``, default: ``100``): default maximum number of bisection iterations.
- ``QuantileRootSolver-PilotSampleSize`` (``UnsignedInteger``, default: ``1000``): default number of pilot output values used to start and scale the bracket.
- ``QuantileRootSolver-DefaultBracketExpansion`` (``UnsignedInteger``, default: ``40``): maximum number of geometric bracket expansions.
- ``QuantileSimulation-DefaultAlpha`` (``Scalar``, default: ``0.95``): default quantile level.
- ``QuantileSimulation-DefaultConfidenceLevel`` (``Scalar``, default: ``0.95``): default nominal confidence level.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.CompositeRandomVector(ot.SymbolicFunction(["x"], ["x"]),
...                                   ot.RandomVector(ot.Normal()))
>>> algo = otexp.LineSamplingQuantile(output, 0.95, 0.95)
>>> algo.setInitialAlpha([1.0])
>>> algo.setSampleSize(10000)
>>> algo.run()
>>> result = algo.getResult()
>>> estimate = result.getQuantileEstimate()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::getInitialAlpha
R"RAW(Accessor to the important direction.

Returns
-------
initialAlpha : :class:`~openturns.Point`
    User-provided important direction.  An empty point means that the
    direction is selected automatically.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::setInitialAlpha
R"RAW(Accessor to the important direction.

Parameters
----------
initialAlpha : sequence of float
    Important direction in the standard space.  It must be finite and
    non-zero and is normalized internally.  When it is not set, the
    direction is computed by FORM at the pilot threshold, with a
    fallback to the extreme pilot standard point.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::getRootStrategy
R"RAW(Accessor to the root strategy.

Returns
-------
rootStrategy : :class:`~openturns.RootStrategy`
    Strategy used to find the intersection of a line with the limit
    state surface.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::setRootStrategy
R"RAW(Accessor to the root strategy.

Parameters
----------
rootStrategy : :class:`~openturns.RootStrategy`
    Strategy used to find the intersection of a line with the limit
    state surface.  Default is :class:`~openturns.SafeAndSlow`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::getSearchOppositeDirection
R"RAW(Accessor to the opposite-direction search flag.

Returns
-------
searchOppositeDirection : bool
    Whether the ray in the direction opposite to the important
    direction is also searched.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::setSearchOppositeDirection
R"RAW(Accessor to the opposite-direction search flag.

Parameters
----------
searchOppositeDirection : bool
    Whether the ray in the direction opposite to the important
    direction is also searched.  Default is
    ``LineSampling-DefaultSearchOppositeDirection``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::getRootTolerance
R"RAW(Accessor to the root-finding tolerance.

Returns
-------
tolerance : float
    Relative width of the final bracket.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::setRootTolerance
R"RAW(Accessor to the root-finding tolerance.

Parameters
----------
tolerance : float
    Relative width of the final bracket.  It must be finite and
    positive.  Default is ``QuantileRootSolver-DefaultTolerance``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::getMaximumRootIteration
R"RAW(Accessor to the maximum number of root-finding iterations.

Returns
-------
maximumIteration : int
    Maximum number of bisection iterations.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::setMaximumRootIteration
R"RAW(Accessor to the maximum number of root-finding iterations.

Parameters
----------
maximumIteration : int
    Maximum number of bisection iterations.  It must be positive.
    Default is ``QuantileRootSolver-DefaultMaximumIteration``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::getPilotSampleSize
R"RAW(Accessor to the pilot sample size.

Returns
-------
pilotSampleSize : int
    Number of output values used to build the initial bracket and,
    when needed, the important direction.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LineSamplingQuantile::setPilotSampleSize
R"RAW(Accessor to the pilot sample size.

Parameters
----------
pilotSampleSize : int
    Number of output values used to build the initial bracket and,
    when needed, the important direction.  It must be positive.
    Default is ``QuantileRootSolver-PilotSampleSize``.
)RAW"
