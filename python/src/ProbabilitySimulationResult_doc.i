%feature("docstring") OT::ProbabilitySimulationResult
R"RAW(Handle to any concrete simulation result, preserving its dynamic type.

This handle stores e.g. :class:`~openturns.NAISResult`,
:class:`~openturns.SubsetSamplingResult` or
:class:`~openturns.CrossEntropyResult` without slicing them. Generic
quantities are readable directly; concrete details are accessible
through ``getImplementation()``, which returns a copy of the concrete
result with its dynamic type.

Available constructors:
    ProbabilitySimulationResult(*result*)

    ProbabilitySimulationResult(*event, probabilityEstimate, varianceEstimate, outerSampling, blockSize*)

Parameters
----------
result : :class:`~openturns.ProbabilitySimulationResultImplementation`
    Concrete simulation result to wrap. It is cloned, so the handle is
    independent of the source object.
event : :class:`~openturns.RandomVector`
    Event we want to evaluate the probability.
probabilityEstimate : float, :math:`0 \leq P_e \leq 1`
    Estimate of the event probability.
varianceEstimate : float, :math:`Var_e \geq 0`
    Variance estimate.
outerSampling : int
    Number of outer sampling.
blockSize : int
    Number of evaluations of the limit state function performed at each
    outer sampling.
)RAW"

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
%feature("docstring") OT::ProbabilitySimulationResult::setOuterSampling
"Accessor to the outer sampling.

Parameters
----------
outerSampling : int
    Outer sampling of the wrapped result
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::setBlockSize
"Accessor to the block size.

Parameters
----------
blockSize : int
    Block size of the wrapped result
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getTimeDuration
"Accessor to the elapsed time.

Returns
-------
timeDuration : float
    Elapsed time of the wrapped result
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::setTimeDuration
"Accessor to the elapsed time.

Parameters
----------
timeDuration : float
    Elapsed time of the wrapped result
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getEvent
"Accessor to event.

Returns
-------
event : :class:`~openturns.RandomVector`
    Event of the wrapped result
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::setEvent
R"RAW(Accessor to the event.

.. deprecated:: 1.28
    The result is now a handle to a
    :class:`~openturns.ProbabilitySimulationResultImplementation`. Set the
    event on the implementation before wrapping it: ``getImplementation()``
    returns a copy, so modifying that copy does not modify the handle.

Parameters
----------
event : :class:`~openturns.RandomVector`
    Event we want to evaluate the probability.
)RAW"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::setProbabilityEstimate
R"RAW(Accessor to the probability estimate.

.. deprecated:: 1.28
    The result is now a handle to a
    :class:`~openturns.ProbabilitySimulationResultImplementation`. Set the
    estimate on the implementation before wrapping it: ``getImplementation()``
    returns a copy, so modifying that copy does not modify the handle.

Parameters
----------
probaEstimate : float, :math:`0 \leq P_e \leq 1`
    Estimate of the event probability.
)RAW"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::setVarianceEstimate
R"RAW(Accessor to the variance estimate.

.. deprecated:: 1.28
    The result is now a handle to a
    :class:`~openturns.ProbabilitySimulationResultImplementation`. Set the
    variance on the implementation before wrapping it: ``getImplementation()``
    returns a copy, so modifying that copy does not modify the handle.

Parameters
----------
varianceEstimate : float, :math:`Var_e \geq 0`
    Variance estimate.
)RAW"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getProbabilityDistribution
"Accessor to the asymptotic probability distribution.

Returns
-------
probaDistribution : :class:`~openturns.Normal`
    Asymptotic normal distribution of the event probability estimate.
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getMeanPointInEventDomain
R"RAW(Accessor to the mean point conditioned to the event realization.

Returns
-------
meanPoint : :class:`~openturns.Point`
    Mean point in the physical space of all the simulations generated by the
    :class:`~openturns.EventSimulation` algorithm that failed into the event
    domain.

Notes
-----
.. warning::
    This notion is only available if the history mechanism of the model is
    activated (see :class:`~openturns.MemoizeFunction`).
)RAW"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getImportanceFactors
R"RAW(Accessor to the importance factors.

.. deprecated:: 1.28
    Use :meth:`~openturns.ProbabilitySimulationResultImplementation.getImportanceFactors`
    on the concrete result returned by :meth:`getImplementation` instead.

Returns
-------
importanceFactors : :class:`~openturns.PointWithDescription`
    Sequence containing the importance factors with a description for each
    component.

Notes
-----
.. warning::
    This notion is only available if the history mechanism of the model is
    activated (see :class:`~openturns.MemoizeFunction`).

See Also
--------
drawImportanceFactors
)RAW"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::drawImportanceFactors
R"RAW(Draw the importance factors.

.. deprecated:: 1.28
    Use :meth:`~openturns.ProbabilitySimulationResultImplementation.drawImportanceFactors`
    on the concrete result returned by :meth:`getImplementation` instead.

.. warning::
    It is necessary to enable the history of the model to perform this analysis
    (see :class:`~openturns.MemoizeFunction`).

Returns
-------
graph : :class:`~openturns.Graph`
    Importance factor graph.

See Also
--------
getImportanceFactors
)RAW"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ProbabilitySimulationResult::getConfidenceLength
R"RAW(Accessor to the confidence length.

.. deprecated:: 1.28
    Use :meth:`~openturns.ProbabilitySimulationResultImplementation.getConfidenceLength`
    on the concrete result returned by :meth:`getImplementation` instead.

Parameters
----------
level : float, optional
    Confidence level, in the range :math:`]0, 1[`.

Returns
-------
length : float
    Length of the bilateral confidence interval of the probability estimate.
)RAW"
