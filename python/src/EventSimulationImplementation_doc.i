%define OT_EventSimulation_doc
R"RAW(Base class for sampling methods.

Parameters
----------
event : :class:`~openturns.RandomVector`
    The event we are computing the probability of.
convergenceStrategy : :class:`~openturns.HistoryStrategy`, optional
    Storage strategy used to store the values of the probability estimator and
    its variance during the simulation algorithm.

Notes
-----
Base class for sampling methods, using the probability distribution of a random
vector :math:`\vect{X}` to evaluate the failure probability:

.. math::

    P_f = \int_{\Rset^{n_X}} \mathbf{1}_{\{g(\ux,\vect{d}) \leq 0 \}}f_{\uX}(\ux)\di{\ux}
        = \Prob{g\left( \vect{X},\vect{d} \right) \leq 0}

Here, :math:`\vect{X}` is a random vector, :math:`\vect{d}` a deterministic
vector, :math:`g(\vect{X},\vect{d})` the function known as *limit state function*
which enables the definition of the event
:math:`\cD_f = \{\vect{X} \in \Rset^n \, | \, g(\vect{X},\vect{d}) \le 0\}`.
:math:`\mathbf{1}_{ \left\{ g(\vect{x}_i,\vect{d}) \leq 0 \right\} }`
describes the indicator function equal to 1 if
:math:`g(\vect{x}_i,\vect{d}) \leq 0` and equal to 0 otherwise.

The EventSimulation object provides a generic simulation service for non-composite
events, and its derived classes provide dedicated algorithms:

- :class:`~openturns.ProbabilitySimulationAlgorithm`

- :class:`~openturns.DirectionalSampling`

- :class:`~openturns.PostAnalyticalControlledImportanceSampling`,

- :class:`~openturns.PostAnalyticalImportanceSampling`,

See also
--------
SimulationResult)RAW"
%enddef
%feature("docstring") OT::EventSimulationImplementation
OT_EventSimulation_doc

// ---------------------------------------------------------------------

%define OT_EventSimulation_getResult_doc
"Accessor to the results.

Returns
-------
results : :class:`~openturns.SimulationResult`
    Structure containing all the results obtained after simulation and created
    by the method :py:meth:`run`."
%enddef
%feature("docstring") OT::EventSimulationImplementation::getResult
OT_EventSimulation_getResult_doc

// ---------------------------------------------------------------------

%define OT_EventSimulation_getEvent_doc
"Accessor to the event.

Returns
-------
event : :class:`~openturns.RandomVector`
    Event we want to evaluate the probability."
%enddef
%feature("docstring") OT::EventSimulationImplementation::getEvent
OT_EventSimulation_getEvent_doc

// ---------------------------------------------------------------------

%define OT_EventSimulation_setEvent_doc
"Accessor to the event.

Parameters
----------
event : :class:`~openturns.RandomVector`
    Event we want to evaluate the probability."
%enddef
%feature("docstring") OT::EventSimulationImplementation::setEvent
OT_EventSimulation_setEvent_doc

// ---------------------------------------------------------------------

%define OT_EventSimulation_run_doc
"Launch simulation.

Notes
-----
It launches the simulation and creates a :class:`~openturns.SimulationResult`,
structure containing all the results obtained after simulation.
It computes the probability of occurrence of the given event by computing the
empirical mean of a sample of size at most *outerSampling * blockSize*,
this sample being built by blocks of size *blockSize*. It allows one to use
efficiently the distribution of the computation as well as it allows one to deal
with a sample size :math:`> 2^{32}` by a combination of *blockSize* and
*outerSampling*.

see also
--------
openturns.EventSimulation.setBlockSize
openturns.EventSimulation.setMaximumOuterSampling
openturns.ResourceMap
openturns.SimulationResult"
%enddef
%feature("docstring") OT::EventSimulationImplementation::run
OT_EventSimulation_run_doc

// ---------------------------------------------------------------------

%define OT_EventSimulation_drawProbabilityConvergence_doc
"Draw the probability convergence at a given level.

Parameters
----------
level : float, optional
    The probability convergence is drawn at this given confidence length
    *level*. By default *level* is 0.95.

Returns
-------
graph : a :class:`~openturns.Graph`
    probability convergence graph"
%enddef
%feature("docstring") OT::EventSimulationImplementation::drawProbabilityConvergence
OT_EventSimulation_drawProbabilityConvergence_doc

// ---------------------------------------------------------------------
%define OT_EventSimulation_setKeepSample_doc
"Accessor to keepSample parameter.

Parameters
----------
keepSample : boolean
    Keep sample indicator
"
%enddef
%feature("docstring") OT::EventSimulationImplementation::setKeepSample
OT_EventSimulation_setKeepSample_doc

// ---------------------------------------------------------------------
%define OT_EventSimulation_getInputSample_doc
"Accessor to input sample.

Returns
-------
inputSample : :class:`~openturns.Sample`
    Input sample
"
%enddef
%feature("docstring") OT::EventSimulationImplementation::getInputSample
OT_EventSimulation_getInputSample_doc
