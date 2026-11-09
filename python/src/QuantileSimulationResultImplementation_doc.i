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
QuantileSimulationResult, QuantileSimulationAlgorithm

Notes
-----
The reported interval is the one returned by the algorithm.  Its
meaning is given by :meth:`getCoverageType`: an exact interval and an
asymptotic interval are not interchangeable, even when they have the
same nominal level ``beta``.

The effective sample size is the Kish effective sample size
:math:`\ESS` of the normalized weights, defined in
:any:`quantile_theory_definitions`.  It equals the sample size when the
weights are uniform.

A result records what the algorithm reports; the class does not
validate that the interval theory is appropriate for the experiment.
A result built by hand can therefore carry an inconsistent estimator
and coverage pair.  Fields that are irrelevant to a given algorithm
keep their default value: for example, the bias estimate is only
present when :meth:`hasBiasEstimate` returns ``True``, and the
convergence history is empty when the algorithm is not iterative.

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
    :math:`N` for uniform weights.
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
