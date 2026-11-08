%feature("docstring") OT::ProbabilityInversionQuantile
R"RAW(Generic probability-inversion quantile estimator.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    ProbabilityInversionQuantile(*algorithm, alpha=0.95, beta=0.95*)

The algorithm estimates the quantile of a scalar output
:math:`Y=g(\mathbf X)` by inverting the probability estimate of an
event-simulation engine.  It is a fallback for engines that expose only
a scalar probability for a threshold event and no sample, weighted
sample or threshold sequence.  For a candidate threshold :math:`t`, the
wrapped engine is cloned, its event is replaced by the threshold event
and it is run; the quantile is the root of

.. math::

    \hat P(t)=1-\alpha,

where :math:`\hat P(t)` is the upper-tail probability returned by the
engine.  A lower-tail event is handled by the same loop.

A plain Monte Carlo engine configured with a
:class:`~openturns.MonteCarloExperiment` is not inverted: the adapter
delegates to :class:`~openturns.experimental.QuantileSimulationAlgorithm` and returns
the exact order-statistic interval.  Other engines are inverted by a
confidence-bound bisection: an endpoint is discarded only when its whole
confidence bound is on one side of the target, and the iteration stops
when the bracket is narrow or when the bound straddles the target.

The adapter never saves or restores the process-wide random generator
state.  It also cannot claim common random numbers unless the wrapped
engine exposes an injectable random stream.  In this release no such
stream capability is available, so a non-delegated result is always
labelled ``COVERAGE_NONE``.  When :meth:`setReplicateCount` is greater
than one, the reported interval is the Student interval on the
independent replicate estimates, but it remains labelled
``COVERAGE_NONE``.

Parameters
----------
algorithm : :class:`~openturns.EventSimulation`
    Event-simulation engine whose probability is inverted.  Its event
    must be a threshold event on a scalar composite random vector.
alpha : float, optional
    Quantile level, in :math:`(0, 1)`.  Default is 0.95.
beta : float, optional
    Confidence level of the interval, in :math:`(0, 1)`.  Default is
    0.95.

Notes
-----
The result returned by :meth:`run` stores the wrapped algorithm
description, the final bracket, the root iteration count, the number of
probability evaluations, the replicate estimates, the common-random-number
diagnostic and the random-stream capability.  A bracket failure raises an
exception: a boundary root or a flat probability surface is never
reported as a silent point estimate.

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
>>> inputDistribution = ot.Normal()
>>> output = ot.CompositeRandomVector(ot.SymbolicFunction(["x"], ["x"]),
...                                   ot.RandomVector(inputDistribution))
>>> event = ot.ThresholdEvent(output, ot.Greater(), 1.0)
>>> engine = ot.ProbabilitySimulationAlgorithm(event)
>>> algo = otexp.ProbabilityInversionQuantile(engine, 0.95, 0.95)
>>> algo.setSampleSize(10000)
>>> algo.run()
>>> result = algo.getResult()
>>> delegated = result.getDelegatedToDirectExtraction()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::getAlgorithm
R"RAW(Accessor to the wrapped algorithm.

Returns
-------
algorithm : :class:`~openturns.EventSimulation`
    Event-simulation engine whose probability is inverted.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::setAlgorithm
R"RAW(Accessor to the wrapped algorithm.

Parameters
----------
algorithm : :class:`~openturns.EventSimulation`
    Event-simulation engine whose probability is inverted.  Its event
    must be a threshold event with a ``Greater``, ``GreaterOrEqual``,
    ``Less`` or ``LessOrEqual`` operator on a scalar composite random
    vector.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::hasAlgorithm
R"RAW(Test whether the wrapped algorithm is set.

Returns
-------
hasAlgorithm : bool
    Whether :meth:`setAlgorithm` has been called.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::getUseCommonRandomNumbers
R"RAW(Accessor to the common-random-number request.

Returns
-------
useCommonRandomNumbers : bool
    Requested use of common random numbers.  This is a request, not a
    capability: the result records whether the coupling was effective.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::setUseCommonRandomNumbers
R"RAW(Accessor to the common-random-number request.

Parameters
----------
useCommonRandomNumbers : bool
    Requested use of common random numbers.  The adapter cannot inject
    a random stream in this release, so the result is still labelled
    ``COVERAGE_NONE``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::getRootTolerance
R"RAW(Accessor to the root-finding tolerance.

Returns
-------
tolerance : float
    Relative width of the final bracket.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::setRootTolerance
R"RAW(Accessor to the root-finding tolerance.

Parameters
----------
tolerance : float
    Relative width of the final bracket.  It must be finite and
    positive.  Default is ``QuantileRootSolver-DefaultTolerance``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::getMaximumRootIteration
R"RAW(Accessor to the maximum number of root-finding iterations.

Returns
-------
maximumIteration : int
    Maximum number of bisection iterations.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::setMaximumRootIteration
R"RAW(Accessor to the maximum number of root-finding iterations.

Parameters
----------
maximumIteration : int
    Maximum number of bisection iterations.  It must be positive.
    Default is ``QuantileRootSolver-DefaultMaximumIteration``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::getPilotSampleSize
R"RAW(Accessor to the pilot sample size.

Returns
-------
pilotSampleSize : int
    Number of output values used to build the initial bracket.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::setPilotSampleSize
R"RAW(Accessor to the pilot sample size.

Parameters
----------
pilotSampleSize : int
    Number of output values used to build the initial bracket.  It must
    be positive.  Default is ``QuantileRootSolver-PilotSampleSize``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::getReplicateCount
R"RAW(Accessor to the replicate count.

Returns
-------
replicateCount : int
    Number of independent complete inversions used to build the
    Student interval.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProbabilityInversionQuantile::setReplicateCount
R"RAW(Accessor to the replicate count.

Parameters
----------
replicateCount : int
    Number of independent complete inversions used to build the
    Student interval.  It must be positive.  Default is 1, which
    reports a degenerate interval.)RAW"
