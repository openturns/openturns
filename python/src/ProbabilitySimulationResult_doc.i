%feature("docstring") OT::ProbabilitySimulationResult
"Handle to any concrete simulation result, preserving its dynamic type.

This handle stores e.g. :class:`~openturns.NAISResult`,
:class:`~openturns.SubsetSamplingResult` or
:class:`~openturns.CrossEntropyResult` without slicing them. Generic
quantities are readable directly; concrete details are accessible
through ``getImplementation()``, which returns the concrete result
with its dynamic type.

Parameters
----------
result : :class:`~openturns.ProbabilitySimulationResultImplementation`
    Concrete simulation result to wrap. It is cloned, so the handle is
    independent of the source object.
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getProbabilityEstimate
"Accessor to probability estimate.

Returns
-------
probabilityEstimate : float
    Probability estimate of the wrapped result
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getVarianceEstimate
"Accessor to variance estimate.

Returns
-------
varianceEstimate : float
    Variance estimate of the wrapped result
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getCoefficientOfVariation
"Accessor to coefficient of variation.

Returns
-------
coefficientOfVariation : float
    Coefficient of variation of the wrapped result
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getStandardDeviation
"Accessor to standard deviation.

Returns
-------
standardDeviation : float
    Standard deviation of the wrapped result
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getOuterSampling
"Accessor to outer sampling.

Returns
-------
outerSampling : int
    Outer sampling of the wrapped result
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getBlockSize
"Accessor to block size.

Returns
-------
blockSize : int
    Block size of the wrapped result
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getEvent
"Accessor to event.

Returns
-------
event : :class:`~openturns.RandomVector`
    Event of the wrapped result
"
