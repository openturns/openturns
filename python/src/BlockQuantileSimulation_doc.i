%feature("docstring") OT::BlockQuantileSimulation
R"RAW(Block quantile simulation.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    BlockQuantileSimulation(*output, alpha=0.95, beta=0.95*)

    BlockQuantileSimulation(*output, experiment, alpha=0.95, beta=0.95*)

The block estimator computes a quantile without ever materializing the
full sample.  The stream of :math:`N` independent outputs is split into
:math:`B` blocks of length :math:`b`, with :math:`N = B b`.  One block
quantile is computed from each block, and the :math:`B` block quantiles
are combined into a single estimate.  Only the :math:`b` input vectors
and :math:`b` scalar outputs of one block, plus the :math:`B` block
quantiles, are kept in memory, so the peak storage is
:math:`\cO((d+1)b + N/b)` scalar values for an input dimension
:math:`d`.

The combination rule is selected by :meth:`setCombinationType`: mean,
median or trimmed mean.  The mean is the default because it has the
smallest asymptotic variance; the median is robust when the block
quantiles are heavy-tailed; the trimmed mean is a compromise controlled
by :meth:`setTrimmingFraction`.  The default confidence interval is the
Student interval on the block quantiles, and a percentile bootstrap
interval is available through :meth:`setBootstrapSize`.  The
combination formulas, the bias-variance trade-off, the block-length
policies and the confidence intervals are presented in
:any:`quantile_theory_block`.

The statistical block length :math:`b` is selected by
:meth:`setBlockLengthPolicy`:

- ``FIXED``: the value set by :meth:`setBlockLength`;
- ``SQRT_N``: :math:`\lceil \sqrt N \rceil`, for a minimal memory
  footprint, with no confidence guarantee;
- ``ASYMPTOTIC``: :math:`\lceil N^{2/3} \rceil`, the default, which
  supports the asymptotic interval;
- ``BIAS_TARGET``: the smallest length for which the estimated bias is
  at most :meth:`setBiasTolerance` times the standard error, raised to
  at least :math:`\lceil \sqrt N \rceil` and capped at
  :math:`N` divided by :meth:`getMinimumBlockCount`;
- ``MEMORY_BOUND``: the largest length compatible with the budget set
  by :meth:`setMemoryBudget`, capped at :math:`N` divided by
  :meth:`getMinimumBlockCount`; the policy throws when the budget
  cannot host that many blocks;
- ``ADAPTIVE``: a pilot-based length with the bias-to-standard-error
  target ``BlockQuantileSimulation-DefaultAdaptiveTolerance`` (0.1 by
  default), subject to the same lower and upper clamps as
  ``BIAS_TARGET``.

The block length :math:`b` is a statistical parameter.  It is distinct
from the inherited evaluation block size: ``setBlockLength(100000)``
computes one block quantile from 100000 points, and the whole block is
evaluated in one call.  The inherited ``setBlockSize()`` does not split
the statistical block into smaller evaluation batches; it is only
reported in the result.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
experiment : :class:`~openturns.WeightedExperiment`
    Independent design of experiments with uniform weights.  Its
    distribution must match the input distribution of ``output``.
    When omitted, a :class:`~openturns.MonteCarloExperiment` is built
    from the distribution of ``output``.
alpha : float
    Quantile level, in :math:`(0, 1)`.  Default is 0.95.
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.
    Default is 0.95.

See also
--------
MonteCarloQuantileSimulation, ExactTailQuantileSimulation, QuantileSimulationResult

Notes
-----
The estimator requires an independent experiment with uniform weights.
Stratified, quasi-Monte Carlo and importance-sampling designs are
rejected because their blocks would not be identically distributed.

The coverage label is ``COVERAGE_ASYMPTOTIC`` when the block length
dominates :math:`\sqrt N`, and ``COVERAGE_NONE`` otherwise.  The
``SQRT_N`` policy always reports ``COVERAGE_NONE`` because its bias and
standard error are of the same order.  The asymptotic label assumes a
smooth output distribution with a positive density at the quantile.
For a discrete or atomic output the block quantiles need not converge
to the quantile, so the label is not a coverage guarantee.  The label is
selected from the block-length policy.  A known discrete output
distribution is reported with ``COVERAGE_NONE``; a general atomic output
cannot be detected from a finite sample.

The block sample contains :math:`Bb` evaluations, reported as the
effective sample size.  The reported sample size also counts the pilot
evaluations drawn by the ``BIAS_TARGET`` and ``ADAPTIVE`` policies, so it
is strictly larger than :math:`Bb` for those policies.  The block-sample
size can be smaller than the requested sample size when :math:`N` is not
a multiple of :math:`b`.

The following :class:`~openturns.ResourceMap` keys are used:

.. list-table::
   :widths: 30 15 15 40
   :header-rows: 1

   * - Key
     - Type
     - Default value
     - Description
   * - ``BlockQuantileSimulation-DefaultAdaptiveTolerance``
     - ``Scalar``
     - ``0.1``
     - Default bias-to-standard-error target of the ``ADAPTIVE`` policy.
   * - ``BlockQuantileSimulation-DefaultBiasTolerance``
     - ``Scalar``
     - ``0.1``
     - Default bias-to-standard-error target of the ``BIAS_TARGET`` policy.
   * - ``BlockQuantileSimulation-DefaultBlockLengthPolicy``
     - ``String``
     - ``ASYMPTOTIC``
     - Default block-length policy; the possible values are ``FIXED``, ``SQRT_N``, ``ASYMPTOTIC``, ``BIAS_TARGET``, ``MEMORY_BOUND`` and ``ADAPTIVE``.
   * - ``BlockQuantileSimulation-DefaultBootstrapSize``
     - ``UnsignedInteger``
     - ``0``
     - Default number of bootstrap replicates, where ``0`` disables the bootstrap.
   * - ``BlockQuantileSimulation-PilotMaximumSize``
     - ``UnsignedInteger``
     - ``20000``
     - Maximum pilot sample size.
   * - ``BlockQuantileSimulation-PilotMinimumSize``
     - ``UnsignedInteger``
     - ``1000``
     - Minimum pilot sample size.
   * - ``BlockQuantileSimulation-PilotSizeFraction``
     - ``Scalar``
     - ``0.05``
     - Pilot sample size as a fraction of the requested sample size.
   * - ``QuantileSimulation-DefaultAlpha``
     - ``Scalar``
     - ``0.95``
     - Default quantile level.
   * - ``QuantileSimulation-DefaultConfidenceLevel``
     - ``Scalar``
     - ``0.95``
     - Default nominal confidence level.
   * - ``QuantileSimulation-DefaultSampleSize``
     - ``UnsignedInteger``
     - ``0``
     - Default sample size, where ``0`` means that the sample size must be set before running.
   * - ``QuantileSimulation-DefaultTrimmingFraction``
     - ``Scalar``
     - ``0.1``
     - Default trimming fraction of the ``TRIMMED_MEAN`` combination rule.
   * - ``QuantileSimulation-MinimumBlockCount``
     - ``UnsignedInteger``
     - ``30``
     - Default minimum number of blocks for the ``MEMORY_BOUND``, ``BIAS_TARGET`` and ``ADAPTIVE`` policies.

See also
--------
MonteCarloQuantileSimulation, ExactTailQuantileSimulation, QuantileSimulationResult

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.RandomVector(ot.Normal())
>>> algo = otexp.BlockQuantileSimulation(output, 0.99, 0.95)
>>> algo.setSampleSize(100000)
>>> algo.setBlockLengthPolicy(otexp.BlockQuantileSimulation.ASYMPTOTIC)
>>> algo.run()
>>> result = algo.getResult()
>>> estimate = result.getQuantileEstimate()
>>> blockLength = result.getBlockLength()
>>> blockCount = result.getBlockCount()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::getExperiment
R"RAW(Accessor to the design of experiments.

Returns
-------
experiment : :class:`~openturns.WeightedExperiment`
    Independent design of experiments with uniform weights.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::setExperiment
R"RAW(Accessor to the design of experiments.

Parameters
----------
experiment : :class:`~openturns.WeightedExperiment`
    Independent design of experiments with uniform weights.  Its
    distribution must match the input distribution of the output random
    vector.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::hasExperiment
R"RAW(Accessor to the presence of a user-provided experiment.

Returns
-------
hasExperiment : bool
    Whether an experiment was provided.  When ``False``, a Monte Carlo
    experiment is built from the output distribution at run time.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::getSampleSize
R"RAW(Accessor to the sample size.

Returns
-------
sampleSize : int
    Requested block-sample size.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::setSampleSize
R"RAW(Accessor to the sample size.

Parameters
----------
sampleSize : int
    Requested block-sample size.  The block sample contains the largest
    multiple of the block length that does not exceed this value.  The
    ``BIAS_TARGET`` and ``ADAPTIVE`` policies draw an additional pilot
    sample, so the total number of model evaluations reported by the
    result can exceed this value.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::getBlockLength
R"RAW(Accessor to the statistical block length.

Returns
-------
blockLength : int
    Statistical block length set by :meth:`setBlockLength`, or 0 when no
    fixed length has been set.  The length actually used by a run is
    reported by :meth:`QuantileSimulationResult.getBlockLength`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::setBlockLength
R"RAW(Accessor to the statistical block length.

Parameters
----------
blockLength : int
    Statistical block length.  It must be positive.  Calling this
    method selects the ``FIXED`` block-length policy.

Notes
-----
The statistical block length is not the evaluation block size.  See
the class documentation.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::getBlockLengthPolicy
R"RAW(Accessor to the block-length policy.

Returns
-------
policy : int
    Policy used to select the statistical block length.  The values are
    ``FIXED``, ``SQRT_N``, ``ASYMPTOTIC``, ``BIAS_TARGET``,
    ``MEMORY_BOUND`` and ``ADAPTIVE``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::setBlockLengthPolicy
R"RAW(Accessor to the block-length policy.

Parameters
----------
policy : int
    Policy used to select the statistical block length.  The values are
    ``FIXED``, ``SQRT_N``, ``ASYMPTOTIC``, ``BIAS_TARGET``,
    ``MEMORY_BOUND`` and ``ADAPTIVE``.

Notes
-----
``FIXED`` uses the value set by :meth:`setBlockLength`.  ``SQRT_N``
uses :math:`\lceil \sqrt N \rceil`.  ``ASYMPTOTIC``, the default, uses
:math:`\lceil N^{2/3} \rceil`.  ``BIAS_TARGET`` and ``ADAPTIVE`` run a
pilot and then select the block length from the estimated bias
constant.  ``MEMORY_BOUND`` uses the budget set by
:meth:`setMemoryBudget`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::getCombinationType
R"RAW(Accessor to the combination rule.

Returns
-------
combination : int
    Rule used to combine the block quantiles.  The values are ``MEAN``,
    ``MEDIAN`` and ``TRIMMED_MEAN``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::setCombinationType
R"RAW(Accessor to the combination rule.

Parameters
----------
combination : int
    Rule used to combine the block quantiles.  The values are ``MEAN``,
    ``MEDIAN`` and ``TRIMMED_MEAN``.  The default is ``MEAN``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::getTrimmingFraction
R"RAW(Accessor to the trimming fraction.

Returns
-------
gamma : float
    Fraction trimmed from each side by the trimmed mean, in
    :math:`[0, 0.5)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::setTrimmingFraction
R"RAW(Accessor to the trimming fraction.

Parameters
----------
gamma : float
    Fraction trimmed from each side by the trimmed mean, in
    :math:`[0, 0.5)`.  Default is 0.1.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::getMinimumBlockCount
R"RAW(Accessor to the minimum block count.

Returns
-------
minimumBlockCount : int
    Minimum number of blocks used by the ``MEMORY_BOUND``,
    ``BIAS_TARGET`` and ``ADAPTIVE`` policy sizing rules.  The
    ``FIXED``, ``SQRT_N`` and ``ASYMPTOTIC`` policies ignore it.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::setMinimumBlockCount
R"RAW(Accessor to the minimum block count.

Parameters
----------
minimumBlockCount : int
    Minimum number of blocks used by the ``MEMORY_BOUND``,
    ``BIAS_TARGET`` and ``ADAPTIVE`` policy sizing rules.  The
    ``FIXED``, ``SQRT_N`` and ``ASYMPTOTIC`` policies ignore it.  It
    must be at least 2.  Default is 30.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::getMemoryBudget
R"RAW(Accessor to the memory budget.

Returns
-------
memoryBudget : int
    Maximum number of scalar values used by the ``MEMORY_BOUND``
    policy, including the input and output block buffers and the block
    quantiles.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::setMemoryBudget
R"RAW(Accessor to the memory budget.

Parameters
----------
memoryBudget : int
    Maximum number of scalar values, including the input and output
    block buffers and the block quantiles.  It must be positive.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::getBiasTolerance
R"RAW(Accessor to the bias tolerance.

Returns
-------
tolerance : float
    Maximum ratio of the estimated bias to the standard error used by
    the ``BIAS_TARGET`` policy.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::setBiasTolerance
R"RAW(Accessor to the bias tolerance.

Parameters
----------
tolerance : float
    Maximum ratio of the estimated bias to the standard error, finite
    and positive.  Default is 0.1.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::getBootstrapSize
R"RAW(Accessor to the bootstrap size.

Returns
-------
bootstrapSize : int
    Number of bootstrap resamples.  A zero value disables the bootstrap
    interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BlockQuantileSimulation::setBootstrapSize
R"RAW(Accessor to the bootstrap size.

Parameters
----------
bootstrapSize : int
    Number of bootstrap resamples.  Use 0 to disable the bootstrap
    interval, or at least 2 to enable it.
)RAW"
