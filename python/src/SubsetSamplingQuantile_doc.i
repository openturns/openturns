%feature("docstring") OT::SubsetSamplingQuantile
R"RAW(Subset-simulation quantile estimator.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    SubsetSamplingQuantile(*output, alpha=0.95, beta=0.95*)

The algorithm estimates the quantile of a scalar output
:math:`Y=\model(\inputRV)` by fixed-level subset simulation [au2001]_.  A
population of particles is propagated through a fixed number of levels:
at each level the particles that do not reach the current threshold are
replaced by draws from the conditional distribution, using a
componentwise random-walk Metropolis-Hastings kernel in the standard
space of the input distribution with a burn-in of :meth:`getBurnIn`
transitions [cerou2007]_.  The whole procedure is repeated
:meth:`getReplicateCount` times with independent random streams; the
estimate is the mean of the replicate estimates and the interval is the
Student interval on the replicate estimates, with asymptotic coverage
in the replicate count.

The level schedule, the conditional quantile estimate and the
confidence interval are described in :any:`quantile_theory_rare_event`.

The particle count is the sample size when :meth:`setSampleSize` has
been called explicitly; otherwise it is the product of the inherited
maximum outer sampling and block size.  The thresholds and level
probabilities of the first replicate are available through
:meth:`QuantileSimulationResult.getThresholdSequence` and
:meth:`QuantileSimulationResult.getLevelProbabilitySequence`; the
replicate estimates and the MCMC acceptance rate are available through
:meth:`QuantileSimulationResult.getReplicateEstimates` and
:meth:`QuantileSimulationResult.getAcceptanceRate`.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
alpha : float
    Quantile level, in :math:`(0, 1)`.  Default is 0.95.
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.
    Default is 0.95.

See also
--------
LastParticleQuantile, CrossEntropyQuantile, QuantileSimulationResult

Notes
-----
The conditional probability and the particle count must satisfy
:math:`p_0 N \ge 1`; otherwise the algorithm cannot retain a particle
at every level.  The conditional probability is also constrained by the
number of levels and by the target conditional level
:math:`p/p_0^{m-1}\le 1`, which the constructor of the level schedule
enforces.

The conditional sampler is defined in the standard space of the input
distribution.  Its proposal range controls the standard deviation of
the componentwise random walk: a range that is too small yields a
slowly mixing chain, while a range that is too large yields a low
acceptance rate.  The reported acceptance rate is the fraction of
proposed transitions that produced a point satisfying the conditional
event.

The following :class:`~openturns.ResourceMap` keys are used:

- ``SubsetSampling-DefaultConditionalProbability`` (``Scalar``, default: ``0.1``): default conditional probability :math:`p_0`.
- ``SubsetSampling-DefaultProposalRange`` (``Scalar``, default: ``2.0``): default random-walk proposal range.
- ``SubsetSamplingQuantile-DefaultBurnIn`` (``UnsignedInteger``, default: ``10``): default number of burn-in transitions.
- ``SubsetSamplingQuantile-DefaultReplicateCount`` (``UnsignedInteger``, default: ``30``): default number of independent replicates.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.RandomVector(ot.Normal())
>>> algo = otexp.SubsetSamplingQuantile(output, 0.95, 0.95)
>>> algo.setSampleSize(500)
>>> algo.setConditionalProbability(0.1)
>>> algo.run()
>>> result = algo.getResult()
>>> estimate = result.getQuantileEstimate()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::getConditionalProbability
R"RAW(Accessor to the conditional probability.

Returns
-------
conditionalProbability : float
    Conditional probability :math:`p_0` used to build the level
    sequence, in :math:`(0, 1)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::setConditionalProbability
R"RAW(Accessor to the conditional probability.

Parameters
----------
conditionalProbability : float
    Conditional probability :math:`p_0` used to build the level
    sequence, in :math:`(0, 1)`.  It must also satisfy
    :math:`p_0 N \ge 1`, where :math:`N` is the particle count.
    Default is 0.1.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::getProposalRange
R"RAW(Accessor to the proposal range.

Returns
-------
proposalRange : float
    Range of the componentwise random-walk proposal used by the
    conditional sampler.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::setProposalRange
R"RAW(Accessor to the proposal range.

Parameters
----------
proposalRange : float
    Range of the componentwise random-walk proposal used by the
    conditional sampler.  It must be finite and positive.  Default is
    2.0.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::getBurnIn
R"RAW(Accessor to the burn-in.

Returns
-------
burnIn : int
    Number of Metropolis-Hastings transitions performed before each
    conditional draw is retained.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::setBurnIn
R"RAW(Accessor to the burn-in.

Parameters
----------
burnIn : int
    Number of Metropolis-Hastings transitions performed before each
    conditional draw is retained.  It must be positive.  Default is 10.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::getReplicateCount
R"RAW(Accessor to the replicate count.

Returns
-------
replicateCount : int
    Number of independent subset runs used to build the Student
    interval.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::setReplicateCount
R"RAW(Accessor to the replicate count.

Parameters
----------
replicateCount : int
    Number of independent subset runs used to build the Student
    interval.  It must be at least 2.  Default is 30.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::getParticleCount
R"RAW(Accessor to the particle count.

Returns
-------
particleCount : int
    Number of particles maintained at each level.  It is the explicitly
    set sample size when available, and the product of the maximum
    outer sampling and block size otherwise.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::setParticleCount
R"RAW(Accessor to the particle count.

Parameters
----------
particleCount : int
    Number of particles maintained at each level.  It is equivalent to
    :meth:`setSampleSize`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::getSampleSize
R"RAW(Accessor to the particle count.

Returns
-------
sampleSize : int
    Number of particles maintained at each level.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SubsetSamplingQuantile::setSampleSize
R"RAW(Accessor to the particle count.

Parameters
----------
sampleSize : int
    Number of particles maintained at each level.  When it is set
    explicitly, it overrides the value deduced from the maximum outer
    sampling and the block size.)RAW"
