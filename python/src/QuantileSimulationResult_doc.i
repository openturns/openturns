%feature("docstring") OT::QuantileSimulationResult
R"RAW(Quantile simulation result.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class gives access to a quantile simulation result: the estimated
quantile, its standard error, the confidence interval, the estimator and
coverage conventions, and the run diagnostics.

See also
--------
QuantileSimulationResultImplementation, MonteCarloQuantileSimulation, BlockQuantileSimulation, ExactTailQuantileSimulation

Notes
-----
The confidence interval is the one returned by the algorithm.  Its
meaning is given by :meth:`getCoverageType`: an exact interval and an
asymptotic interval are not interchangeable, even when they have the
same nominal level ``beta``.

Fields that are irrelevant to a given algorithm keep their default
value: for example, the bias estimate is only present when
:meth:`hasBiasEstimate` returns ``True``, and the convergence history is
empty when the algorithm is not iterative.

Block algorithms add the block length, the number of blocks, the block
quantile sample, the combination rule, the peak memory estimate and,
when a pilot is available, the estimated bias.  Exact-tail algorithms
add the retained tail, the number of retained order statistics, and the
zero-based exact ranks used by the binomial interval.  Threshold
algorithms add the threshold sequence, the level probability sequence,
the number of steps, the MCMC acceptance rate, the particle and
replicate counts, the replicate estimates, the Poisson ranks of the
last-particle method, and the auxiliary distribution and intermediate
quantile level of the cross-entropy method.

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
>>> algo = otexp.MonteCarloQuantileSimulation(output, 0.95, 0.95)
>>> algo.setSampleSize(1000)
>>> algo.run()
>>> result = algo.getResult()
>>> alpha = result.getQuantileLevel()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getOutput
R"RAW(Accessor to the output random vector.

Returns
-------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setOutput
R"RAW(Accessor to the output random vector.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getQuantileLevel
R"RAW(Accessor to the quantile level.

Returns
-------
alpha : float
    Quantile level, in :math:`(0, 1)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setQuantileLevel
R"RAW(Accessor to the quantile level.

Parameters
----------
alpha : float
    Quantile level, in :math:`(0, 1)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getConfidenceLevel
R"RAW(Accessor to the nominal confidence level.

Returns
-------
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setConfidenceLevel
R"RAW(Accessor to the nominal confidence level.

Parameters
----------
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getMasterSeed
R"RAW(Accessor to the master seed.

Returns
-------
seed : int
    Seed from which the run's random streams are derived.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setMasterSeed
R"RAW(Accessor to the master seed.

Parameters
----------
seed : int
    Seed from which the run's random streams are derived.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasMasterSeed
R"RAW(Accessor to the master seed availability.

Returns
-------
hasMasterSeed : bool
    Whether a master seed was recorded for the run.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getQuantileEstimate
R"RAW(Accessor to the quantile estimate.

Returns
-------
estimate : float
    Estimated quantile at level :meth:`getQuantileLevel`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setQuantileEstimate
R"RAW(Accessor to the quantile estimate.

Parameters
----------
estimate : float
    Estimated quantile at level :meth:`getQuantileLevel`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getStandardError
R"RAW(Accessor to the standard error.

Returns
-------
standardError : float
    Estimated standard error of the quantile estimate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setStandardError
R"RAW(Accessor to the standard error.

Parameters
----------
standardError : float
    Estimated standard error of the quantile estimate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getConfidenceInterval
R"RAW(Accessor to the confidence interval.

Returns
-------
interval : :class:`~openturns.Interval`
    Confidence interval at level :meth:`getConfidenceLevel`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setConfidenceInterval
R"RAW(Accessor to the confidence interval.

Parameters
----------
interval : :class:`~openturns.Interval`
    One-dimensional confidence interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getEstimatorType
R"RAW(Accessor to the estimator type.

Returns
-------
estimatorType : int
    Estimator convention, either
    ``QuantileSimulationResultImplementation.EMPIRICAL_INVERSE`` or
    ``QuantileSimulationResultImplementation.INTERPOLATED``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setEstimatorType
R"RAW(Accessor to the estimator type.

Parameters
----------
estimatorType : int
    Estimator convention, either
    ``QuantileSimulationResultImplementation.EMPIRICAL_INVERSE`` or
    ``QuantileSimulationResultImplementation.INTERPOLATED``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getCoverageType
R"RAW(Accessor to the achieved coverage type.

Returns
-------
coverageType : int
    Coverage guarantee actually provided by the reported interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setCoverageType
R"RAW(Accessor to the achieved coverage type.

Parameters
----------
coverageType : int
    Coverage guarantee actually provided by the reported interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getRequiredCoverage
R"RAW(Accessor to the required coverage type.

Returns
-------
requiredCoverage : int
    Coverage guarantee requested by the user.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setRequiredCoverage
R"RAW(Accessor to the required coverage type.

Parameters
----------
requiredCoverage : int
    Coverage guarantee requested by the user.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getEffectiveSampleSize
R"RAW(Accessor to the effective sample size.

Returns
-------
effectiveSampleSize : float
    Kish effective sample size of the normalized weights, in
    :math:`(0, N]`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setEffectiveSampleSize
R"RAW(Accessor to the effective sample size.

Parameters
----------
effectiveSampleSize : float
    Kish effective sample size of the normalized weights.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getSampleSize
R"RAW(Accessor to the sample size.

Returns
-------
sampleSize : int
    Number of model evaluations actually performed.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setSampleSize
R"RAW(Accessor to the sample size.

Parameters
----------
sampleSize : int
    Number of model evaluations actually performed.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getBlockLength
R"RAW(Accessor to the statistical block length.

Returns
-------
blockLength : int
    Block length used by a block algorithm; 0 when the algorithm is not
    block based.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setBlockLength
R"RAW(Accessor to the statistical block length.

Parameters
----------
blockLength : int
    Block length used by a block algorithm; 0 when the algorithm is not
    block based.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getBlockCount
R"RAW(Accessor to the number of blocks.

Returns
-------
blockCount : int
    Number of independent blocks combined by a block algorithm; 0 when
    the algorithm is not block based.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setBlockCount
R"RAW(Accessor to the number of blocks.

Parameters
----------
blockCount : int
    Number of independent blocks combined by a block algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getBlockQuantileSample
R"RAW(Accessor to the block quantile sample.

Returns
-------
blockQuantiles : :class:`~openturns.Point`
    Quantile computed from each block.  The size of the point is the
    number of blocks; it is empty when the algorithm is not block
    based.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setBlockQuantileSample
R"RAW(Accessor to the block quantile sample.

Parameters
----------
blockQuantiles : sequence of float
    Quantile computed from each block.  The values must be finite.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getCombinationType
R"RAW(Accessor to the combination rule.

Returns
-------
combination : int
    Rule used to combine the block quantiles.  The values are ``MEAN``,
    ``MEDIAN`` and ``TRIMMED_MEAN``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setCombinationType
R"RAW(Accessor to the combination rule.

Parameters
----------
combination : int
    Rule used to combine the block quantiles.  The values are ``MEAN``,
    ``MEDIAN`` and ``TRIMMED_MEAN``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getCombinationDescription
R"RAW(Accessor to the combination rule description.

Returns
-------
description : str
    Human-readable description of the combination rule, including the
    trimming fraction for the trimmed mean.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getTrimmingFraction
R"RAW(Accessor to the trimming fraction.

Returns
-------
gamma : float
    Fraction trimmed from each side by the trimmed mean, in
    :math:`[0, 0.5)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setTrimmingFraction
R"RAW(Accessor to the trimming fraction.

Parameters
----------
gamma : float
    Fraction trimmed from each side by the trimmed mean, in
    :math:`[0, 0.5)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getBlockLengthPolicyDescription
R"RAW(Accessor to the block-length policy description.

Returns
-------
description : str
    Human-readable description of the policy that selected the block
    length.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setBlockLengthPolicyDescription
R"RAW(Accessor to the block-length policy description.

Parameters
----------
description : str
    Human-readable description of the policy that selected the block
    length.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getPeakMemory
R"RAW(Accessor to the peak memory estimate.

Returns
-------
peakMemory : int
    Peak number of scalar values stored by the algorithm.  For the
    block estimator it is :math:`(d+1)` times the block length plus the
    number of block quantiles, where :math:`d` is the input dimension;
    for the exact-tail estimator it is the number of retained order
    statistics plus the input and output samples of one chunk.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setPeakMemory
R"RAW(Accessor to the peak memory estimate.

Parameters
----------
peakMemory : int
    Peak number of scalar values stored by the algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getPeakMemoryInBytes
R"RAW(Accessor to the peak memory estimate in bytes.

Returns
-------
peakMemoryInBytes : int
    Peak memory estimate, assuming eight bytes per scalar value.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getBiasEstimate
R"RAW(Accessor to the bias estimate.

Returns
-------
biasEstimate : float
    Estimated bias of the quantile estimate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setBiasEstimate
R"RAW(Accessor to the bias estimate.

Parameters
----------
biasEstimate : float
    Estimated bias of the quantile estimate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasBiasEstimate
R"RAW(Accessor to the bias estimate availability.

Returns
-------
hasBiasEstimate : bool
    Whether a bias estimate was recorded for the run.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getBiasToStandardErrorRatio
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

%feature("docstring") OT::QuantileSimulationResult::getTailType
R"RAW(Accessor to the retained tail.

Returns
-------
tail : int
    Tail retained by an exact-tail algorithm.  The values are
    ``NO_TAIL``, ``LOWER_TAIL`` and ``UPPER_TAIL``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setTailType
R"RAW(Accessor to the retained tail.

Parameters
----------
tail : int
    Tail retained by an exact-tail algorithm.  The values are
    ``NO_TAIL``, ``LOWER_TAIL`` and ``UPPER_TAIL``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getTailDescription
R"RAW(Accessor to the retained tail description.

Returns
-------
description : str
    Human-readable description of the retained tail.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getRetainedCount
R"RAW(Accessor to the retained order-statistic count.

Returns
-------
retainedCount : int
    Number of order statistics kept by an exact-tail algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setRetainedCount
R"RAW(Accessor to the retained order-statistic count.

Parameters
----------
retainedCount : int
    Number of order statistics kept by an exact-tail algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getLowerRank
R"RAW(Accessor to the lower exact rank.

Returns
-------
lowerRank : int
    Zero-based index of the lower order statistic of the exact binomial
    interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setLowerRank
R"RAW(Accessor to the lower exact rank.

Parameters
----------
lowerRank : int
    Zero-based index of the lower order statistic of the exact binomial
    interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getUpperRank
R"RAW(Accessor to the upper exact rank.

Returns
-------
upperRank : int
    Zero-based index of the upper order statistic of the exact binomial
    interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setUpperRank
R"RAW(Accessor to the upper exact rank.

Parameters
----------
upperRank : int
    Zero-based index of the upper order statistic of the exact binomial
    interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getThresholdSequence
R"RAW(Accessor to the threshold sequence.

Returns
-------
thresholds : :class:`~openturns.Point`
    Intermediate thresholds in the order in which they were generated:
    the subset levels of the first replicate for
    :class:`~openturns.experimental.SubsetSamplingQuantile`, the
    successive extreme values for
    :class:`~openturns.experimental.LastParticleQuantile`, and the
    cross-entropy thresholds for
    :class:`~openturns.experimental.CrossEntropyQuantile`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setThresholdSequence
R"RAW(Accessor to the threshold sequence.

Parameters
----------
thresholds : sequence of float
    Intermediate thresholds.  All the values must be finite.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getLevelProbabilitySequence
R"RAW(Accessor to the level probability sequence.

Returns
-------
probabilities : :class:`~openturns.Point`
    Probability of the event associated with each threshold, in the
    same order as :meth:`getThresholdSequence`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setLevelProbabilitySequence
R"RAW(Accessor to the level probability sequence.

Parameters
----------
probabilities : sequence of float
    Estimated probability of the event associated with each threshold.
    All the values must lie in :math:`[0,1]`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getStepCount
R"RAW(Accessor to the number of steps.

Returns
-------
stepCount : int
    Number of levels or moves performed by the threshold algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setStepCount
R"RAW(Accessor to the number of steps.

Parameters
----------
stepCount : int
    Number of levels or moves performed by the threshold algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getAcceptanceRate
R"RAW(Accessor to the MCMC acceptance rate.

Returns
-------
acceptanceRate : float
    Fraction of proposed Metropolis-Hastings transitions that produced
    a point satisfying the conditional event.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setAcceptanceRate
R"RAW(Accessor to the MCMC acceptance rate.

Parameters
----------
acceptanceRate : float
    Fraction of accepted transitions, in :math:`[0,1]`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getPoissonRankLower
R"RAW(Accessor to the lower Poisson rank.

Returns
-------
lowerRank : int
    One-based index of the lower threshold of the Poisson rank
    interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getPoissonRankUpper
R"RAW(Accessor to the upper Poisson rank.

Returns
-------
upperRank : int
    One-based index of the upper threshold of the Poisson rank
    interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setPoissonRanks
R"RAW(Accessor to the Poisson rank interval.

Parameters
----------
lowerRank : int
    One-based index of the lower threshold of the Poisson rank
    interval.  It must be positive.
upperRank : int
    One-based index of the upper threshold of the Poisson rank
    interval.  It must not be smaller than ``lowerRank``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasPoissonRanks
R"RAW(Accessor to the presence of Poisson ranks.

Returns
-------
hasPoissonRanks : bool
    Whether the result stores a Poisson rank interval.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getParticleCount
R"RAW(Accessor to the particle count.

Returns
-------
particleCount : int
    Number of particles used by the threshold algorithm, or number of
    auxiliary draws for the cross-entropy estimator.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setParticleCount
R"RAW(Accessor to the particle count.

Parameters
----------
particleCount : int
    Number of particles or auxiliary draws.  It must be positive.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getReplicateCount
R"RAW(Accessor to the replicate count.

Returns
-------
replicateCount : int
    Number of independent replicates used by the estimator.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setReplicateCount
R"RAW(Accessor to the replicate count.

Parameters
----------
replicateCount : int
    Number of independent replicates used by the estimator.  It must be
    positive.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getReplicateEstimates
R"RAW(Accessor to the replicate estimates.

Returns
-------
estimates : :class:`~openturns.Point`
    Quantile estimate produced by each independent replicate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setReplicateEstimates
R"RAW(Accessor to the replicate estimates.

Parameters
----------
estimates : sequence of float
    Quantile estimate produced by each independent replicate.  All the
    values must be finite.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getAuxiliaryDistribution
R"RAW(Accessor to the auxiliary distribution.

Returns
-------
distribution : :class:`~openturns.Distribution`
    Proposal distribution used by the final weighted estimate of the
    cross-entropy estimator.  It is defined on the standard space of
    the input distribution and is the defensive proposal derived from
    the final cross-entropy auxiliary distribution.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setAuxiliaryDistribution
R"RAW(Accessor to the auxiliary distribution.

Parameters
----------
distribution : :class:`~openturns.Distribution`
    Proposal distribution used by the final weighted estimate of the
    cross-entropy estimator.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasAuxiliaryDistribution
R"RAW(Accessor to the presence of an auxiliary distribution.

Returns
-------
hasAuxiliaryDistribution : bool
    Whether the result stores an auxiliary distribution.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getIntermediateQuantileLevel
R"RAW(Accessor to the intermediate quantile level.

Returns
-------
level : float
    Intermediate quantile level of the cross-entropy sequence.  It is
    distinct from the target quantile level :math:`\alpha`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setIntermediateQuantileLevel
R"RAW(Accessor to the intermediate quantile level.

Parameters
----------
level : float
    Intermediate quantile level of the cross-entropy sequence, in
    :math:`(0,1)`.  It is distinct from the target quantile level
    :math:`\alpha`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getConditionalProbability
R"RAW(Accessor to the conditional probability.

Returns
-------
conditionalProbability : float
    Conditional probability :math:`p_0` of the subset estimator.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setConditionalProbability
R"RAW(Accessor to the conditional probability.

Parameters
----------
conditionalProbability : float
    Conditional probability :math:`p_0` of the subset estimator, in
    :math:`(0,1)`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::isFixedSample
R"RAW(Accessor to the fixed-sample flag.

Returns
-------
isFixedSample : bool
    Whether the run used a fixed sample size.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setIsFixedSample
R"RAW(Accessor to the fixed-sample flag.

Parameters
----------
isFixedSample : bool
    Whether the run used a fixed sample size.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getConvergenceHistory
R"RAW(Accessor to the convergence history.

Returns
-------
history : :class:`~openturns.Sample`
    Convergence history with at least two columns: the cumulative
    sample size and the quantile estimate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setConvergenceHistory
R"RAW(Accessor to the convergence history.

Parameters
----------
history : :class:`~openturns.Sample`
    Convergence history with at least two columns: the cumulative
    sample size and the quantile estimate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getOuterSampling
R"RAW(Accessor to the outer sampling count.

Returns
-------
outerSampling : int
    Number of independent experiment draws or iterations performed.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setOuterSampling
R"RAW(Accessor to the outer sampling count.

Parameters
----------
outerSampling : int
    Number of independent experiment draws or iterations performed.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getBlockSize
R"RAW(Accessor to the evaluation block size.

Returns
-------
blockSize : int
    Number of model evaluations performed in a single call.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setBlockSize
R"RAW(Accessor to the evaluation block size.

Parameters
----------
blockSize : int
    Number of model evaluations performed in a single call.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getTimeDuration
R"RAW(Accessor to the elapsed time.

Returns
-------
timeDuration : float
    Elapsed wall-clock time of the run, in seconds.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setTimeDuration
R"RAW(Accessor to the elapsed time.

Parameters
----------
timeDuration : float
    Elapsed wall-clock time of the run, in seconds.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getEstimatorDescription
R"RAW(Accessor to the estimator description.

Returns
-------
description : str
    Human-readable name of the estimator convention.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getCoverageDescription
R"RAW(Accessor to the coverage description.

Returns
-------
description : str
    Human-readable name of the achieved coverage guarantee.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::drawQuantileConvergence
R"RAW(Draw the quantile convergence.

Returns
-------
graph : :class:`~openturns.Graph`
    Graph of the quantile estimate against the cumulative sample size.
)RAW"

%feature("docstring") OT::QuantileSimulationResult::getReplicateCount
R"RAW(Accessor to the replicate count.

Returns
-------
replicateCount : int
    Number of independent replicates used by the estimator.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setReplicateCount
R"RAW(Accessor to the replicate count.

Parameters
----------
replicateCount : int
    Number of independent replicates used by the estimator.  It must be
    positive.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getReplicateEstimates
R"RAW(Accessor to the replicate estimates.

Returns
-------
estimates : :class:`~openturns.Point`
    Quantile estimate produced by each independent replicate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setReplicateEstimates
R"RAW(Accessor to the replicate estimates.

Parameters
----------
estimates : sequence of float
    Quantile estimate produced by each independent replicate.  All the
    values must be finite.
)RAW"
%feature("docstring") OT::QuantileSimulationResult::getDirectionCount
R"RAW(Accessor to the direction count.

Returns
-------
directionCount : int
    Number of directions used by the directional estimator, or zero
    when the estimator is not directional.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setDirectionCount
R"RAW(Accessor to the direction count.

Parameters
----------
directionCount : int
    Number of directions used by the directional estimator.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getConditionalProbabilitySample
R"RAW(Accessor to the conditional-probability sample.

Returns
-------
probabilities : :class:`~openturns.Point`
    Per-direction or per-line conditional probabilities evaluated at the
    returned quantile.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setConditionalProbabilitySample
R"RAW(Accessor to the conditional-probability sample.

Parameters
----------
probabilities : sequence of float
    Per-direction or per-line conditional probabilities evaluated at the
    returned quantile.  They must lie in :math:`[0,1]`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getImportantDirection
R"RAW(Accessor to the important direction.

Returns
-------
direction : :class:`~openturns.Point`
    Important direction used by the line-sampling estimator.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setImportantDirection
R"RAW(Accessor to the important direction.

Parameters
----------
direction : sequence of float
    Important direction used by the line-sampling estimator.  It must be
    finite and non-zero.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasImportantDirection
R"RAW(Test whether an important direction is available.

Returns
-------
hasImportantDirection : bool
    Whether the result stores an important direction.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getRootBracket
R"RAW(Accessor to the root bracket.

Returns
-------
bracket : :class:`~openturns.Interval`
    Final bracket of the root-finding algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setRootBracket
R"RAW(Accessor to the root bracket.

Parameters
----------
bracket : :class:`~openturns.Interval`
    Final bracket of the root-finding algorithm.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasRootBracket
R"RAW(Test whether a root bracket is available.

Returns
-------
hasRootBracket : bool
    Whether the result stores a root bracket.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getRootIterationCount
R"RAW(Accessor to the root iteration count.

Returns
-------
iterationCount : int
    Number of bisection iterations performed by the root finder.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setRootIterationCount
R"RAW(Accessor to the root iteration count.

Parameters
----------
iterationCount : int
    Number of bisection iterations performed by the root finder.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getProbabilityEvaluationCount
R"RAW(Accessor to the probability evaluation count.

Returns
-------
evaluationCount : int
    Number of probability evaluations performed by the root finder.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setProbabilityEvaluationCount
R"RAW(Accessor to the probability evaluation count.

Parameters
----------
evaluationCount : int
    Number of probability evaluations performed by the root finder.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getRootSolverType
R"RAW(Accessor to the root-solver type.

Returns
-------
type : int
    Either ``BISECTION`` or ``ROBBINS_MONRO``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setRootSolverType
R"RAW(Accessor to the root-solver type.

Parameters
----------
type : int
    Either ``BISECTION`` or ``ROBBINS_MONRO``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getRootSolverDescription
R"RAW(Accessor to the root-solver description.

Returns
-------
description : str
    ``"Bisection"`` or ``"Robbins-Monro"``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getCommonRandomNumbers
R"RAW(Accessor to the common-random-number diagnostic.

Returns
-------
commonRandomNumbers : bool
    Whether the root-finding probability evaluations are effectively
    coupled by a common random stream.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setCommonRandomNumbers
R"RAW(Accessor to the common-random-number diagnostic.

Parameters
----------
commonRandomNumbers : bool
    Whether the root-finding probability evaluations are effectively
    coupled by a common random stream.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getRandomStreamCapability
R"RAW(Accessor to the random-stream capability.

Returns
-------
capability : bool
    Whether the wrapped algorithm can inject a random stream.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setRandomStreamCapability
R"RAW(Accessor to the random-stream capability.

Parameters
----------
capability : bool
    Whether the wrapped algorithm can inject a random stream.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getDelegatedToDirectExtraction
R"RAW(Accessor to the direct-extraction delegation flag.

Returns
-------
delegated : bool
    Whether the inversion was delegated to direct extraction from a
    sample.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setDelegatedToDirectExtraction
R"RAW(Accessor to the direct-extraction delegation flag.

Parameters
----------
delegated : bool
    Whether the inversion was delegated to direct extraction from a
    sample.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getWrappedAlgorithmDescription
R"RAW(Accessor to the wrapped algorithm description.

Returns
-------
description : str
    Description of the event-simulation engine wrapped by the inversion.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setWrappedAlgorithmDescription
R"RAW(Accessor to the wrapped algorithm description.

Parameters
----------
description : str
    Description of the event-simulation engine wrapped by the inversion.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasWrappedAlgorithmDescription
R"RAW(Test whether a wrapped algorithm description is available.

Returns
-------
hasWrappedAlgorithmDescription : bool
    Whether the result stores a wrapped algorithm description.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getAnalyticalProbability
R"RAW(Accessor to the analytical probability.

Returns
-------
probability : float
    FORM or SORM probability evaluated at the returned quantile.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setAnalyticalProbability
R"RAW(Accessor to the analytical probability.

Parameters
----------
probability : float
    FORM or SORM probability evaluated at the returned quantile, in
    :math:`[0,1]`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasAnalyticalProbability
R"RAW(Test whether an analytical probability is available.

Returns
-------
hasAnalyticalProbability : bool
    Whether the result stores an analytical probability.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getDesignPoint
R"RAW(Accessor to the design point.

Returns
-------
designPoint : :class:`~openturns.Point`
    Standard-space design point of the analytical approximation.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setDesignPoint
R"RAW(Accessor to the design point.

Parameters
----------
designPoint : sequence of float
    Standard-space design point of the analytical approximation.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasDesignPoint
R"RAW(Test whether a design point is available.

Returns
-------
hasDesignPoint : bool
    Whether the result stores a design point.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getReliabilityIndex
R"RAW(Accessor to the reliability index.

Returns
-------
reliabilityIndex : float
    Hasofer-Lind reliability index of the analytical approximation.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setReliabilityIndex
R"RAW(Accessor to the reliability index.

Parameters
----------
reliabilityIndex : float
    Hasofer-Lind reliability index of the analytical approximation.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasReliabilityIndex
R"RAW(Test whether a reliability index is available.

Returns
-------
hasReliabilityIndex : bool
    Whether the result stores a reliability index.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getCurvatureCorrection
R"RAW(Accessor to the curvature correction.

Returns
-------
correction : float
    Ratio between the SORM probability and the FORM probability.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setCurvatureCorrection
R"RAW(Accessor to the curvature correction.

Parameters
----------
correction : float
    Ratio between the SORM probability and the FORM probability.  It
    must be finite and positive.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasCurvatureCorrection
R"RAW(Test whether a curvature correction is available.

Returns
-------
hasCurvatureCorrection : bool
    Whether the result stores a curvature correction.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getRefinedProbability
R"RAW(Accessor to the refined probability.

Returns
-------
probability : float
    Probability obtained by the importance-sampling refinement.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setRefinedProbability
R"RAW(Accessor to the refined probability.

Parameters
----------
probability : float
    Probability obtained by the importance-sampling refinement, in
    :math:`[0,1]`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::hasRefinedProbability
R"RAW(Test whether a refined probability is available.

Returns
-------
hasRefinedProbability : bool
    Whether the result stores a refined probability.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::getRefinementStandardError
R"RAW(Accessor to the refinement standard error.

Returns
-------
standardError : float
    Standard error of the importance-sampling refinement.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationResult::setRefinementStandardError
R"RAW(Accessor to the refinement standard error.

Parameters
----------
standardError : float
    Standard error of the importance-sampling refinement.  It must be
    finite and non-negative.
)RAW"

// ---------------------------------------------------------------------