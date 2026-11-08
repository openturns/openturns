%feature("docstring") OT::DirectionalSamplingQuantile
R"RAW(Directional-sampling quantile estimator.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    DirectionalSamplingQuantile(*output, alpha=0.95, beta=0.95*)

The algorithm estimates the quantile of a scalar output
:math:`Y=g(\mathbf X)` by a deterministic bisection on the directional
probability.  The input random vector is mapped to the standard space
and a direction sample :math:`(\mathbf u_1,\dots,\mathbf u_N)` is drawn
once.  For a candidate threshold :math:`t`, the conditional probability
along the ray :math:`r\mathbf u_i` is computed from the roots of
:math:`g(r\mathbf u_i)=t`, and the directional estimate is

.. math::

    \hat P_N(t) = \frac{1}{N}\sum_{i=1}^N \hat p_i(t),

where :math:`\hat p_i(t)` is the standard normal probability mass past
the first root.  The directions are not redrawn when :math:`t` changes:
this structural common-random-number coupling makes the outer objective
smooth enough for bisection.  The root is bracketed geometrically around
a pilot quantile and then bisected to :meth:`getRootTolerance`.

The confidence interval follows from the delta method applied to the
conditional contributions :math:`\hat p_i(t)`.  Its coverage is
asymptotic, and it is labelled ``COVERAGE_NONE`` when the derivative is
not valid or when the root did not converge.

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
The sample size is the number of directions and must be at least two for
the delta-method interval.

The result returned by :meth:`run` stores the direction count, the
conditional-probability sample, the root bracket, the root iteration
count and the number of probability evaluations.  The common-random-number
diagnostic is set to ``True`` and the random-stream capability to
``False``: the coupling is structural, not stream based.

The following :class:`~openturns.ResourceMap` keys are used:

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
>>> algo = otexp.DirectionalSamplingQuantile(output, 0.95, 0.95)
>>> algo.setSampleSize(10000)
>>> algo.run()
>>> result = algo.getResult()
>>> estimate = result.getQuantileEstimate()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DirectionalSamplingQuantile::getRootStrategy
R"RAW(Accessor to the root strategy.

Returns
-------
rootStrategy : :class:`~openturns.RootStrategy`
    Strategy used to find the intersection of a ray with the limit
    state surface.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DirectionalSamplingQuantile::setRootStrategy
R"RAW(Accessor to the root strategy.

Parameters
----------
rootStrategy : :class:`~openturns.RootStrategy`
    Strategy used to find the intersection of a ray with the limit
    state surface.  Default is :class:`~openturns.SafeAndSlow`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DirectionalSamplingQuantile::getSamplingStrategy
R"RAW(Accessor to the sampling strategy.

Returns
-------
samplingStrategy : :class:`~openturns.SamplingStrategy`
    Strategy used to draw the directions on the unit sphere.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DirectionalSamplingQuantile::setSamplingStrategy
R"RAW(Accessor to the sampling strategy.

Parameters
----------
samplingStrategy : :class:`~openturns.SamplingStrategy`
    Strategy used to draw the directions on the unit sphere.  Default is
    :class:`~openturns.RandomDirection`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DirectionalSamplingQuantile::getRootTolerance
R"RAW(Accessor to the root-finding tolerance.

Returns
-------
tolerance : float
    Relative width of the final bracket.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DirectionalSamplingQuantile::setRootTolerance
R"RAW(Accessor to the root-finding tolerance.

Parameters
----------
tolerance : float
    Relative width of the final bracket.  It must be finite and
    positive.  Default is ``QuantileRootSolver-DefaultTolerance``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DirectionalSamplingQuantile::getMaximumRootIteration
R"RAW(Accessor to the maximum number of root-finding iterations.

Returns
-------
maximumIteration : int
    Maximum number of bisection iterations.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DirectionalSamplingQuantile::setMaximumRootIteration
R"RAW(Accessor to the maximum number of root-finding iterations.

Parameters
----------
maximumIteration : int
    Maximum number of bisection iterations.  It must be positive.
    Default is ``QuantileRootSolver-DefaultMaximumIteration``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DirectionalSamplingQuantile::getPilotSampleSize
R"RAW(Accessor to the pilot sample size.

Returns
-------
pilotSampleSize : int
    Number of output values used to build the initial bracket.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DirectionalSamplingQuantile::setPilotSampleSize
R"RAW(Accessor to the pilot sample size.

Parameters
----------
pilotSampleSize : int
    Number of output values used to build the initial bracket.  It must
    be positive.  Default is ``QuantileRootSolver-PilotSampleSize``.)RAW"
