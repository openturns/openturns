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
QuantileSimulationResultImplementation, QuantileSimulationAlgorithm

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

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.RandomVector(ot.Normal())
>>> algo = otexp.QuantileSimulationAlgorithm(output, 0.95, 0.95)
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
