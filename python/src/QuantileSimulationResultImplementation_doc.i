%feature("docstring") OT::QuantileSimulationResultImplementation
R"RAW(Quantile simulation result.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class describes a quantile simulation result: the estimated
quantile, its standard error, the confidence interval, the estimator and
coverage conventions, and the run diagnostics.

The estimator convention is reported through :meth:`getEstimatorType`:

- ``QuantileSimulationResultImplementation.EMPIRICAL_INVERSE``: the
  reported value is an order statistic of the sample;
- ``QuantileSimulationResultImplementation.INTERPOLATED``: the reported
  value interpolates neighbouring order statistics.

The coverage guarantee is reported through :meth:`getCoverageType`:

- ``QuantileSimulationResultImplementation.COVERAGE_EXACT``: the
  confidence interval has a distribution-free exact coverage;
- ``QuantileSimulationResultImplementation.COVERAGE_ASYMPTOTIC``: the
  coverage is asymptotic and relies on a central limit theorem;
- ``QuantileSimulationResultImplementation.COVERAGE_ANYTIME_VALID``:
  the coverage holds uniformly over the stopping rule;
- ``QuantileSimulationResultImplementation.COVERAGE_NONE``: no
  confidence guarantee is claimed.

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
QuantileSimulationResult, QuantileSimulationAlgorithm, BlockQuantileSimulation, ExactTailQuantileSimulation

Notes
-----
The reported interval is the one returned by the algorithm.  Its
meaning is given by :meth:`getCoverageType`: an exact interval and an
asymptotic interval are not interchangeable, even when they have the
same nominal level ``beta``.

The effective sample size is the Kish effective sample size
:math:`\ESS` of the normalized weights, defined in
:any:`quantile_theory_definitions`.  It equals the sample size when the
weights are uniform.  For
:class:`~openturns.experimental.TwoStageQuantileSimulation` it counts
only the confirmation observations, while the sample size also counts
the pilot evaluations.

A result records what the algorithm reports; the class does not
validate that the interval theory is appropriate for the experiment.
A result built by hand can therefore carry an inconsistent estimator
and coverage pair.  Fields that are irrelevant to a given algorithm
keep their default value: for example, the bias estimate is only
present when :meth:`hasBiasEstimate` returns ``True``, and the
convergence history is empty when the algorithm is not iterative.

Block algorithms use :meth:`getBlockLength`, :meth:`getBlockCount`,
:meth:`getBlockQuantileSample`, :meth:`getCombinationType`,
:meth:`getTrimmingFraction` and :meth:`getPeakMemory`.  The combination
rule is one of ``MEAN``, ``MEDIAN`` and ``TRIMMED_MEAN``.  When a pilot
or an adaptive policy provides a bias estimate, the ratio
:meth:`getBiasToStandardErrorRatio` is the quantity that controls the
coverage error of the asymptotic interval.

Exact-tail algorithms use :meth:`getTailType`, :meth:`getRetainedCount`,
:meth:`getLowerRank` and :meth:`getUpperRank`.  The tail is one of
``NO_TAIL``, ``LOWER_TAIL`` and ``UPPER_TAIL``.  The ranks are zero-based
order-statistic indices: the exact interval is
:math:`[Y_{(r_1+1)}, Y_{(r_2+1)}]` in one-based notation.

The following :class:`~openturns.ResourceMap` keys are used:

- ``QuantileSimulation-DefaultAlpha`` (``Scalar``, default: ``0.95``): default quantile level.
- ``QuantileSimulation-DefaultConfidenceLevel`` (``Scalar``, default: ``0.95``): default nominal confidence level.
- ``QuantileSimulation-DefaultSampleSize`` (``UnsignedInteger``, default: ``0``): default sample size, where ``0`` means that the sample size is not set.
- ``QuantileSimulation-DefaultTrimmingFraction`` (``Scalar``, default: ``0.1``): default trimming fraction of the ``TRIMMED_MEAN`` combination rule.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.RandomVector(ot.Normal())
>>> algo = otexp.QuantileSimulationAlgorithm(output, 0.95, 0.95)
>>> algo.setSampleSize(1000)
>>> algo.run()
>>> result = algo.getResult()
>>> estimate = result.getQuantileEstimate()
>>> interval = result.getConfidenceInterval()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getOutput
R"RAW(Accessor to the output random vector.

Returns
-------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setOutput
R"RAW(Accessor to the output random vector.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.

Notes
-----
The random vector must be scalar: its dimension must be equal to 1.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getQuantileLevel
R"RAW(Accessor to the quantile level.

Returns
-------
alpha : float
    Quantile level, in :math:`(0, 1)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setQuantileLevel
R"RAW(Accessor to the quantile level.

Parameters
----------
alpha : float
    Quantile level, in :math:`(0, 1)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getConfidenceLevel
R"RAW(Accessor to the nominal confidence level.

Returns
-------
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setConfidenceLevel
R"RAW(Accessor to the nominal confidence level.

Parameters
----------
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getMasterSeed
R"RAW(Accessor to the master seed.

Returns
-------
seed : int
    Seed from which the run's random streams are derived.

Notes
-----
The value is only meaningful when :meth:`hasMasterSeed` returns
``True``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setMasterSeed
R"RAW(Accessor to the master seed.

Parameters
----------
seed : int
    Seed from which the run's random streams are derived.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::hasMasterSeed
R"RAW(Accessor to the master seed availability.

Returns
-------
hasMasterSeed : bool
    Whether a master seed was recorded for the run.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getQuantileEstimate
R"RAW(Accessor to the quantile estimate.

Returns
-------
estimate : float
    Estimated quantile at level :meth:`getQuantileLevel`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setQuantileEstimate
R"RAW(Accessor to the quantile estimate.

Parameters
----------
estimate : float
    Estimated quantile at level :meth:`getQuantileLevel`.  It must be
    finite.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getStandardError
R"RAW(Accessor to the standard error.

Returns
-------
standardError : float
    Estimated standard error of the quantile estimate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setStandardError
R"RAW(Accessor to the standard error.

Parameters
----------
standardError : float
    Estimated standard error of the quantile estimate.  It must be
    finite and non-negative.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getConfidenceInterval
R"RAW(Accessor to the confidence interval.

Returns
-------
interval : :class:`~openturns.Interval`
    Confidence interval at level :meth:`getConfidenceLevel`.  Its
    coverage guarantee is given by :meth:`getCoverageType`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setConfidenceInterval
R"RAW(Accessor to the confidence interval.

Parameters
----------
interval : :class:`~openturns.Interval`
    One-dimensional confidence interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getEstimatorType
R"RAW(Accessor to the estimator type.

Returns
-------
estimatorType : int
    Estimator convention, either
    ``QuantileSimulationResultImplementation.EMPIRICAL_INVERSE`` or
    ``QuantileSimulationResultImplementation.INTERPOLATED``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setEstimatorType
R"RAW(Accessor to the estimator type.

Parameters
----------
estimatorType : int
    Estimator convention, either
    ``QuantileSimulationResultImplementation.EMPIRICAL_INVERSE`` or
    ``QuantileSimulationResultImplementation.INTERPOLATED``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getCoverageType
R"RAW(Accessor to the achieved coverage type.

Returns
-------
coverageType : int
    Coverage guarantee actually provided by the reported interval.
    See the class documentation for the possible values.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setCoverageType
R"RAW(Accessor to the achieved coverage type.

Parameters
----------
coverageType : int
    Coverage guarantee actually provided by the reported interval.
    See the class documentation for the possible values.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getRequiredCoverage
R"RAW(Accessor to the required coverage type.

Returns
-------
requiredCoverage : int
    Coverage guarantee requested by the user.  A run fails when the
    achieved coverage does not satisfy it.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setRequiredCoverage
R"RAW(Accessor to the required coverage type.

Parameters
----------
requiredCoverage : int
    Coverage guarantee requested by the user.  A run fails when the
    achieved coverage does not satisfy it.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getEffectiveSampleSize
R"RAW(Accessor to the effective sample size.

Returns
-------
effectiveSampleSize : float
    Kish effective sample size of the normalized weights, in
    :math:`(0, N]`, where :math:`N` is the sample size.  It equals
    :math:`N` for uniform weights, except for
    :class:`~openturns.experimental.TwoStageQuantileSimulation`, which
    records the pilot and confirmation evaluations in the sample size
    but only the confirmation observations in the effective sample size.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setEffectiveSampleSize
R"RAW(Accessor to the effective sample size.

Parameters
----------
effectiveSampleSize : float
    Kish effective sample size of the normalized weights.  It must be
    finite and positive.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getSampleSize
R"RAW(Accessor to the sample size.

Returns
-------
sampleSize : int
    Number of model evaluations actually performed.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setSampleSize
R"RAW(Accessor to the sample size.

Parameters
----------
sampleSize : int
    Number of model evaluations actually performed.  It must be
    positive.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getBlockLength
R"RAW(Accessor to the statistical block length.

Returns
-------
blockLength : int
    Block length used by a block algorithm; 0 when the algorithm is not
    block based.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setBlockLength
R"RAW(Accessor to the statistical block length.

Parameters
----------
blockLength : int
    Block length used by a block algorithm; 0 when the algorithm is not
    block based.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getBlockCount
R"RAW(Accessor to the number of blocks.

Returns
-------
blockCount : int
    Number of independent blocks combined by a block algorithm; 0 when
    the algorithm is not block based.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setBlockCount
R"RAW(Accessor to the number of blocks.

Parameters
----------
blockCount : int
    Number of independent blocks combined by a block algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getBlockQuantileSample
R"RAW(Accessor to the block quantile sample.

Returns
-------
blockQuantiles : :class:`~openturns.Point`
    Quantile computed from each block.  The size of the point is the
    number of blocks; it is empty when the algorithm is not block
    based.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setBlockQuantileSample
R"RAW(Accessor to the block quantile sample.

Parameters
----------
blockQuantiles : sequence of float
    Quantile computed from each block.  The values must be finite.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getCombinationType
R"RAW(Accessor to the combination rule.

Returns
-------
combination : int
    Rule used to combine the block quantiles.  The values are ``MEAN``,
    ``MEDIAN`` and ``TRIMMED_MEAN``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setCombinationType
R"RAW(Accessor to the combination rule.

Parameters
----------
combination : int
    Rule used to combine the block quantiles.  The values are ``MEAN``,
    ``MEDIAN`` and ``TRIMMED_MEAN``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getCombinationDescription
R"RAW(Accessor to the combination rule description.

Returns
-------
description : str
    Human-readable description of the combination rule, including the
    trimming fraction for the trimmed mean.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getTrimmingFraction
R"RAW(Accessor to the trimming fraction.

Returns
-------
gamma : float
    Fraction trimmed from each side by the trimmed mean, in
    :math:`[0, 0.5)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setTrimmingFraction
R"RAW(Accessor to the trimming fraction.

Parameters
----------
gamma : float
    Fraction trimmed from each side by the trimmed mean, in
    :math:`[0, 0.5)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getBlockLengthPolicyDescription
R"RAW(Accessor to the block-length policy description.

Returns
-------
description : str
    Human-readable description of the policy that selected the block
    length.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setBlockLengthPolicyDescription
R"RAW(Accessor to the block-length policy description.

Parameters
----------
description : str
    Human-readable description of the policy that selected the block
    length.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getPeakMemory
R"RAW(Accessor to the peak memory estimate.

Returns
-------
peakMemory : int
    Peak number of scalar values stored by the algorithm.  For the
    block estimator it is the block length plus the number of block
    quantiles; for the exact-tail estimator it is the number of retained
    order statistics.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setPeakMemory
R"RAW(Accessor to the peak memory estimate.

Parameters
----------
peakMemory : int
    Peak number of scalar values stored by the algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getPeakMemoryInBytes
R"RAW(Accessor to the peak memory estimate in bytes.

Returns
-------
peakMemoryInBytes : int
    Peak memory estimate, assuming eight bytes per scalar value.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getBiasEstimate
R"RAW(Accessor to the bias estimate.

Returns
-------
biasEstimate : float
    Estimated bias of the quantile estimate.  The value is only
    meaningful when :meth:`hasBiasEstimate` returns ``True``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setBiasEstimate
R"RAW(Accessor to the bias estimate.

Parameters
----------
biasEstimate : float
    Estimated bias of the quantile estimate.  It must be finite.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::hasBiasEstimate
R"RAW(Accessor to the bias estimate availability.

Returns
-------
hasBiasEstimate : bool
    Whether a bias estimate was recorded for the run.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getBiasToStandardErrorRatio
R"RAW(Accessor to the bias-to-standard-error ratio.

Returns
-------
ratio : float
    Absolute value of the estimated bias divided by the standard error.
    It is zero when no bias estimate is available.  Values below 0.1
    indicate a negligible bias; values above 0.5 indicate that the
    nominal interval should not be trusted.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getTailType
R"RAW(Accessor to the retained tail.

Returns
-------
tail : int
    Tail retained by an exact-tail algorithm.  The values are
    ``NO_TAIL``, ``LOWER_TAIL`` and ``UPPER_TAIL``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setTailType
R"RAW(Accessor to the retained tail.

Parameters
----------
tail : int
    Tail retained by an exact-tail algorithm.  The values are
    ``NO_TAIL``, ``LOWER_TAIL`` and ``UPPER_TAIL``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getTailDescription
R"RAW(Accessor to the retained tail description.

Returns
-------
description : str
    Human-readable description of the retained tail.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getRetainedCount
R"RAW(Accessor to the retained order-statistic count.

Returns
-------
retainedCount : int
    Number of order statistics kept by an exact-tail algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setRetainedCount
R"RAW(Accessor to the retained order-statistic count.

Parameters
----------
retainedCount : int
    Number of order statistics kept by an exact-tail algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getLowerRank
R"RAW(Accessor to the lower exact rank.

Returns
-------
lowerRank : int
    Zero-based index of the lower order statistic of the exact binomial
    interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setLowerRank
R"RAW(Accessor to the lower exact rank.

Parameters
----------
lowerRank : int
    Zero-based index of the lower order statistic of the exact binomial
    interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getUpperRank
R"RAW(Accessor to the upper exact rank.

Returns
-------
upperRank : int
    Zero-based index of the upper order statistic of the exact binomial
    interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setUpperRank
R"RAW(Accessor to the upper exact rank.

Parameters
----------
upperRank : int
    Zero-based index of the upper order statistic of the exact binomial
    interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::isFixedSample
R"RAW(Accessor to the fixed-sample flag.

Returns
-------
isFixedSample : bool
    Whether the run used a fixed sample size rather than an adaptive
    stopping rule.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setIsFixedSample
R"RAW(Accessor to the fixed-sample flag.

Parameters
----------
isFixedSample : bool
    Whether the run used a fixed sample size rather than an adaptive
    stopping rule.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getConvergenceHistory
R"RAW(Accessor to the convergence history.

Returns
-------
history : :class:`~openturns.Sample`
    Convergence history with at least two columns: the cumulative
    sample size and the quantile estimate.  When the algorithm reports
    confidence bounds, columns 2 and 3 contain the lower and upper
    bounds.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setConvergenceHistory
R"RAW(Accessor to the convergence history.

Parameters
----------
history : :class:`~openturns.Sample`
    Convergence history with at least two columns: the cumulative
    sample size and the quantile estimate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getEstimatorDescription
R"RAW(Accessor to the estimator description.

Returns
-------
description : str
    Human-readable name of the estimator convention.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getCoverageDescription
R"RAW(Accessor to the coverage description.

Returns
-------
description : str
    Human-readable name of the achieved coverage guarantee.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::drawQuantileConvergence
R"RAW(Draw the quantile convergence.

Returns
-------
graph : :class:`~openturns.Graph`
    Graph of the quantile estimate against the cumulative sample size.
    When the convergence history contains confidence bounds, they are
    drawn as two additional curves.
)RAW"

%feature("docstring") OT::QuantileSimulationResultImplementation::getReplicateCount
R"RAW(Accessor to the replicate count.

Returns
-------
replicateCount : int
    Number of independent replicates used by the estimator.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setReplicateCount
R"RAW(Accessor to the replicate count.

Parameters
----------
replicateCount : int
    Number of independent replicates used by the estimator.  It must be
    positive.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getReplicateEstimates
R"RAW(Accessor to the replicate estimates.

Returns
-------
estimates : :class:`~openturns.Point`
    Quantile estimate produced by each independent replicate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setReplicateEstimates
R"RAW(Accessor to the replicate estimates.

Parameters
----------
estimates : sequence of float
    Quantile estimate produced by each independent replicate.  All the
    values must be finite.
)RAW"

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getPilotSize
R"RAW(Accessor to the pilot size of a two-stage design.

Returns
-------
pilotSize : int
    Number of observations drawn in the pilot stage.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setPilotSize
R"RAW(Accessor to the pilot size of a two-stage design.

Parameters
----------
pilotSize : int
    Number of observations drawn in the pilot stage.  It must be
    positive.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getPilotQuantileEstimate
R"RAW(Accessor to the pilot quantile estimate.

Returns
-------
estimate : float
    Quantile estimated on the pilot sample of a two-stage design.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setPilotQuantileEstimate
R"RAW(Accessor to the pilot quantile estimate.

Parameters
----------
estimate : float
    Quantile estimated on the pilot sample of a two-stage design.  It
    must be finite.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::hasPilotQuantileEstimate
R"RAW(Test whether a pilot quantile estimate is available.

Returns
-------
hasPilotQuantileEstimate : bool
    Whether the pilot quantile estimate has been set.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getPilotDensity
R"RAW(Accessor to the pilot density estimate.

Returns
-------
density : float
    Spacing estimate of the density at the quantile, computed on the
    pilot sample of a two-stage design.  Zero means that the spacing
    was degenerate.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setPilotDensity
R"RAW(Accessor to the pilot density estimate.

Parameters
----------
density : float
    Spacing estimate of the density at the quantile.  It must be finite
    and non-negative.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::hasPilotDensity
R"RAW(Test whether a pilot density estimate is available.

Returns
-------
hasPilotDensity : bool
    Whether the pilot density estimate has been set.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getConfirmationSize
R"RAW(Accessor to the confirmation size of a two-stage design.

Returns
-------
confirmationSize : int
    Number of observations actually drawn in the confirmation stage.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setConfirmationSize
R"RAW(Accessor to the confirmation size of a two-stage design.

Parameters
----------
confirmationSize : int
    Number of observations actually drawn in the confirmation stage.
    It must be positive.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getRequiredConfirmationSize
R"RAW(Accessor to the confirmation size required by the sizing rule.

Returns
-------
requiredConfirmationSize : int
    Confirmation size requested by the two-stage sizing formula before
    the budget clamp.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setRequiredConfirmationSize
R"RAW(Accessor to the confirmation size required by the sizing rule.

Parameters
----------
requiredConfirmationSize : int
    Confirmation size requested by the two-stage sizing formula.  It
    must be positive.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getTargetHalfWidth
R"RAW(Accessor to the target half-width.

Returns
-------
targetHalfWidth : float
    Target half-width requested for a two-stage or sequential design.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setTargetHalfWidth
R"RAW(Accessor to the target half-width.

Parameters
----------
targetHalfWidth : float
    Target half-width requested for a two-stage or sequential design.
    It must be finite and positive.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::hasTargetHalfWidth
R"RAW(Test whether a target half-width is recorded.

Returns
-------
hasTargetHalfWidth : bool
    Whether a positive target half-width has been set.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getBudgetExhausted
R"RAW(Accessor to the budget exhaustion flag.

Returns
-------
budgetExhausted : bool
    Whether the evaluation budget was reached before the target
    half-width was met.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setBudgetExhausted
R"RAW(Accessor to the budget exhaustion flag.

Parameters
----------
budgetExhausted : bool
    Whether the evaluation budget was reached before the target
    half-width was met.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getStoppingTime
R"RAW(Accessor to the stopping time of a sequential design.

Returns
-------
stoppingTime : int
    Number of observations drawn when the sequential algorithm
    stopped.  It is zero when no sequential run has been recorded.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setStoppingTime
R"RAW(Accessor to the stopping time of a sequential design.

Parameters
----------
stoppingTime : int
    Number of observations drawn when the sequential algorithm stopped.
    It must be positive.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getStoppingCriterion
R"RAW(Accessor to the stopping criterion of a sequential design.

Returns
-------
stoppingCriterion : str
    ``"target half-width"`` when the run stopped because the interval
    became short enough, ``"maximum sample size"`` when it stopped at
    the evaluation budget, and an empty string when no sequential run
    has been recorded.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setStoppingCriterion
R"RAW(Accessor to the stopping criterion of a sequential design.

Parameters
----------
stoppingCriterion : str
    Stopping criterion.  It must not be empty.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getConfidenceSequencePath
R"RAW(Accessor to the confidence sequence path of a sequential design.

Returns
-------
path : :class:`~openturns.Sample`
    Path with one row per evaluated sample size and four columns: the
    sample size, the quantile estimate, the lower bound and the upper
    bound.  Infinite bounds are represented by
    :math:`\pm` ``SpecFunc.MaxScalar``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setConfidenceSequencePath
R"RAW(Accessor to the confidence sequence path of a sequential design.

Parameters
----------
path : :class:`~openturns.Sample`
    Path with four columns: sample size, estimate, lower bound and
    upper bound.  It must be a finite sample with four columns when it
    is not empty.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::getCoverageAssumption
R"RAW(Accessor to the coverage assumption of a sequential design.

Returns
-------
coverageAssumption : str
    Distribution-free assumption that supports the anytime-valid
    coverage claim, or an empty string when no such claim is made.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResultImplementation::setCoverageAssumption
R"RAW(Accessor to the coverage assumption of a sequential design.

Parameters
----------
coverageAssumption : str
    Distribution-free assumption that supports the anytime-valid
    coverage claim.  It must not be empty.)RAW"
