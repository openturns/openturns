%feature("docstring") OT::QuantileSimulation
R"RAW(Quantile simulation algorithm.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class represents a quantile simulation algorithm: it stores the
output random vector, the quantile and confidence levels, the sample
size, the estimator and coverage conventions, and it returns a
:class:`~openturns.experimental.QuantileSimulationResult`.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
alpha : float
    Quantile level, in :math:`(0, 1)`.  Default is 0.95.
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.
    Default is 0.95.

Notes
-----
The weighted-experiment estimator is described in
:class:`~openturns.experimental.QuantileSimulationAlgorithm`.

The convergence controls (``maximumOuterSampling``,
``maximumCoefficientOfVariation``, ``maximumStandardDeviation``,
``maximumTimeDuration`` and ``convergenceStrategy``), the evaluation
block size and the callbacks have no effect on this estimator: the
number of evaluations is fixed by the experiment size, the replicate
count and :meth:`setSampleSize`.

See also
--------
QuantileSimulationImplementation, QuantileSimulationAlgorithm, QuantileSimulationResult

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.RandomVector(ot.Normal())
>>> algo = otexp.QuantileSimulationAlgorithm(output, 0.95, 0.95)
>>> algo.setSampleSize(1000)
>>> algo.run()
>>> result = algo.getResult()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getOutput
R"RAW(Accessor to the output random vector.

Returns
-------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setOutput
R"RAW(Accessor to the output random vector.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getAlpha
R"RAW(Accessor to the quantile level.

Returns
-------
alpha : float
    Quantile level, in :math:`(0, 1)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setAlpha
R"RAW(Accessor to the quantile level.

Parameters
----------
alpha : float
    Quantile level, in :math:`(0, 1)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getConfidenceLevel
R"RAW(Accessor to the nominal confidence level.

Returns
-------
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setConfidenceLevel
R"RAW(Accessor to the nominal confidence level.

Parameters
----------
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getSampleSize
R"RAW(Accessor to the sample size.

Returns
-------
sampleSize : int
    Number of model evaluations performed by one experiment draw.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setSampleSize
R"RAW(Accessor to the sample size.

Parameters
----------
sampleSize : int
    Number of model evaluations performed by one experiment draw.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getEstimatorType
R"RAW(Accessor to the estimator type.

Returns
-------
estimatorType : int
    Estimator convention, either
    ``QuantileSimulationResultImplementation.EMPIRICAL_INVERSE`` or
    ``QuantileSimulationResultImplementation.INTERPOLATED``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setEstimatorType
R"RAW(Accessor to the estimator type.

Parameters
----------
estimatorType : int
    Estimator convention, either
    ``QuantileSimulationResultImplementation.EMPIRICAL_INVERSE`` or
    ``QuantileSimulationResultImplementation.INTERPOLATED``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getRequiredCoverage
R"RAW(Accessor to the required coverage type.

Returns
-------
requiredCoverage : int
    Coverage guarantee requested by the user.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setRequiredCoverage
R"RAW(Accessor to the required coverage type.

Parameters
----------
requiredCoverage : int
    Coverage guarantee requested by the user.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::run
R"RAW(Perform the quantile simulation.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getResult
R"RAW(Accessor to the simulation result.

Returns
-------
result : :class:`~openturns.experimental.QuantileSimulationResult`
    Result of the last run.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getMaximumOuterSampling
R"RAW(Accessor to the maximum number of outer iterations.

Returns
-------
maximumOuterSampling : int
    Maximum number of outer iterations.  It is updated when the sample
    size changes, but the weighted-experiment algorithm does not use it
    as a stopping criterion.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setMaximumOuterSampling
R"RAW(Accessor to the maximum number of outer iterations.

Parameters
----------
maximumOuterSampling : int
    Maximum number of outer iterations.  The weighted-experiment
    algorithm stores it but does not use it as a stopping criterion.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getMaximumCoefficientOfVariation
R"RAW(Accessor to the maximum coefficient of variation.

Returns
-------
maximumCoefficientOfVariation : float
    Maximum coefficient of variation.  The weighted-experiment algorithm
    does not use it: the number of evaluations is fixed by the experiment
    size, the replicate count and the sample size.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setMaximumCoefficientOfVariation
R"RAW(Accessor to the maximum coefficient of variation.

Parameters
----------
maximumCoefficientOfVariation : float
    Maximum coefficient of variation.  The weighted-experiment algorithm
    does not use it: the number of evaluations is fixed by the experiment
    size, the replicate count and the sample size.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getMaximumStandardDeviation
R"RAW(Accessor to the maximum standard deviation.

Returns
-------
maximumStandardDeviation : float
    Maximum standard deviation.  The weighted-experiment algorithm does
    not use it: the number of evaluations is fixed by the experiment
    size, the replicate count and the sample size.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setMaximumStandardDeviation
R"RAW(Accessor to the maximum standard deviation.

Parameters
----------
maximumStandardDeviation : float
    Maximum standard deviation.  The weighted-experiment algorithm does
    not use it: the number of evaluations is fixed by the experiment
    size, the replicate count and the sample size.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getBlockSize
R"RAW(Accessor to the evaluation block size.

Returns
-------
blockSize : int
    Evaluation block size.  It is reported in the result but does not
    split the experiment draw.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setBlockSize
R"RAW(Accessor to the evaluation block size.

Parameters
----------
blockSize : int
    Evaluation block size.  It is reported in the result but does not
    split the experiment draw.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getMaximumTimeDuration
R"RAW(Accessor to the maximum time duration.

Returns
-------
maximumTimeDuration : float
    Maximum time duration.  The weighted-experiment algorithm does not
    use it as a stopping criterion.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setMaximumTimeDuration
R"RAW(Accessor to the maximum time duration.

Parameters
----------
maximumTimeDuration : float
    Maximum time duration.  The weighted-experiment algorithm does not
    use it as a stopping criterion.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::getConvergenceStrategy
R"RAW(Accessor to the convergence strategy.

Returns
-------
convergenceStrategy : :class:`~openturns.HistoryStrategy`
    Convergence strategy.  The weighted-experiment algorithm does not
    use it: the convergence history is built from the quantile estimates
    of the successive draws.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setConvergenceStrategy
R"RAW(Accessor to the convergence strategy.

Parameters
----------
convergenceStrategy : :class:`~openturns.HistoryStrategy`
    Convergence strategy.  The weighted-experiment algorithm does not
    use it: the convergence history is built from the quantile estimates
    of the successive draws.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setProgressCallback
R"RAW(Set up a progress callback.

Parameters
----------
callback : callable
    Function called with the progress percentage, as a float.

Notes
-----
The weighted-experiment algorithm does not invoke the callback.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulation::setStopCallback
R"RAW(Set up a stop callback.

Parameters
----------
callback : callable
    Function returning whether the run must stop.

Notes
-----
The weighted-experiment algorithm does not invoke the callback.)RAW"
