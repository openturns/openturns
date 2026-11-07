%feature("docstring") OT::QuantileSimulationImplementation
R"RAW(Quantile simulation algorithm.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class describes a quantile simulation algorithm: it stores the
scalar output random vector, the quantile level :math:`\alpha`, the
nominal confidence level :math:`\beta`, the sample size, the estimator
convention, the required coverage, and the result of the last run.

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
The estimator convention is either
``QuantileSimulationResultImplementation.EMPIRICAL_INVERSE`` or
``QuantileSimulationResultImplementation.INTERPOLATED``.  It determines
which convention is reported in the result.

The required coverage is either
``QuantileSimulationResultImplementation.COVERAGE_NONE`` (no
requirement, the default), ``COVERAGE_EXACT``,
``COVERAGE_ASYMPTOTIC`` or ``COVERAGE_ANYTIME_VALID``.  A run throws
when the achieved coverage does not satisfy the requirement; an exact
interval satisfies an asymptotic requirement.

Calling :meth:`run` on a directly constructed
:class:`~openturns.experimental.QuantileSimulationImplementation` raises
``NotYetImplementedException``.  Use
:class:`~openturns.experimental.QuantileSimulationAlgorithm` for the
weighted-experiment algorithm.

The convergence controls, the evaluation block size and the callbacks
have no effect on the weighted-experiment estimator: the number of
evaluations is fixed by the experiment size, the replicate count and
:meth:`setSampleSize`.

See also
--------
QuantileSimulation, QuantileSimulationAlgorithm, QuantileSimulationResult

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

%feature("docstring") OT::QuantileSimulationImplementation::getOutput
R"RAW(Accessor to the output random vector.

Returns
-------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::setOutput
R"RAW(Accessor to the output random vector.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.

Notes
-----
The random vector must be scalar: its dimension must be equal to 1.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::getAlpha
R"RAW(Accessor to the quantile level.

Returns
-------
alpha : float
    Quantile level, in :math:`(0, 1)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::setAlpha
R"RAW(Accessor to the quantile level.

Parameters
----------
alpha : float
    Quantile level, in :math:`(0, 1)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::getConfidenceLevel
R"RAW(Accessor to the nominal confidence level.

Returns
-------
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::setConfidenceLevel
R"RAW(Accessor to the nominal confidence level.

Parameters
----------
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::getSampleSize
R"RAW(Accessor to the sample size.

Returns
-------
sampleSize : int
    Number of model evaluations performed by one experiment draw.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::setSampleSize
R"RAW(Accessor to the sample size.

Parameters
----------
sampleSize : int
    Number of model evaluations performed by one experiment draw.
    It must be positive.

Notes
-----
The maximum number of outer iterations is updated to
``ceil(sampleSize / blockSize)``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::getEstimatorType
R"RAW(Accessor to the estimator type.

Returns
-------
estimatorType : int
    Estimator convention, either
    ``QuantileSimulationResultImplementation.EMPIRICAL_INVERSE`` or
    ``QuantileSimulationResultImplementation.INTERPOLATED``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::setEstimatorType
R"RAW(Accessor to the estimator type.

Parameters
----------
estimatorType : int
    Estimator convention, either
    ``QuantileSimulationResultImplementation.EMPIRICAL_INVERSE`` or
    ``QuantileSimulationResultImplementation.INTERPOLATED``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::getRequiredCoverage
R"RAW(Accessor to the required coverage type.

Returns
-------
requiredCoverage : int
    Coverage guarantee requested by the user.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::setRequiredCoverage
R"RAW(Accessor to the required coverage type.

Parameters
----------
requiredCoverage : int
    Coverage guarantee requested by the user, either
    ``QuantileSimulationResultImplementation.COVERAGE_NONE``,
    ``COVERAGE_EXACT``, ``COVERAGE_ASYMPTOTIC`` or
    ``COVERAGE_ANYTIME_VALID``.

Notes
-----
A run throws when the achieved coverage does not satisfy the required
coverage.  An exact interval satisfies an asymptotic requirement.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::run
R"RAW(Perform the quantile simulation.

The method validates the inputs, performs the simulation, and verifies
the required coverage.

Raises
------
InvalidArgumentException
    If the output is not scalar, if the levels are outside
    :math:`(0, 1)`, if the sample size is not set, or if the achieved
    coverage does not satisfy the required coverage.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::getResult
R"RAW(Accessor to the simulation result.

Returns
-------
result : :class:`~openturns.experimental.QuantileSimulationResult`
    Result of the last run.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationImplementation::setResult
R"RAW(Accessor to the simulation result.

Parameters
----------
result : :class:`~openturns.experimental.QuantileSimulationResult`
    Result of the last run.)RAW"
