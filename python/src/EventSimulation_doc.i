%feature("docstring") OT::EventSimulation
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

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::getResult
"Accessor to the results.

Returns
-------
results : :class:`~openturns.SimulationResult`
    Structure containing all the results obtained after simulation and created
    by the method :py:meth:`run`."

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::getEvent
"Accessor to the event.

Returns
-------
event : :class:`~openturns.RandomVector`
    Event we want to evaluate the probability."

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::setEvent
"Accessor to the event.

Parameters
----------
event : :class:`~openturns.RandomVector`
    Event we want to evaluate the probability."

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::run
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

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::drawProbabilityConvergence
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
    
// ---------------------------------------------------------------------
%feature("docstring") OT::EventSimulation::setKeepSample
"Accessor to keepSample parameter.

Parameters
----------
keepSample : boolean
    Keep sample indicator
"

// ---------------------------------------------------------------------
%feature("docstring") OT::EventSimulation::getInputSample
"Accessor to input sample.

Returns
-------
inputSample : :class:`~openturns.Sample`
    Input sample
"

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::getBlockSize
OT_SimulationAlgorithm_getBlockSize_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::setBlockSize
OT_SimulationAlgorithm_setBlockSize_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::getMaximumCoefficientOfVariation
OT_SimulationAlgorithm_getMaximumCoefficientOfVariation_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::setMaximumCoefficientOfVariation
OT_SimulationAlgorithm_setMaximumCoefficientOfVariation_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::getMaximumOuterSampling
OT_SimulationAlgorithm_getMaximumOuterSampling_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::setMaximumOuterSampling
OT_SimulationAlgorithm_setMaximumOuterSampling_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::getMaximumStandardDeviation
OT_SimulationAlgorithm_getMaximumStandardDeviation_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::setMaximumStandardDeviation
OT_SimulationAlgorithm_setMaximumStandardDeviation_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::getConvergenceStrategy
OT_SimulationAlgorithm_getConvergenceStrategy_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::setConvergenceStrategy
OT_SimulationAlgorithm_setConvergenceStrategy_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::setMaximumTimeDuration
OT_SimulationAlgorithm_setMaximumTimeDuration_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::getMaximumTimeDuration
OT_SimulationAlgorithm_getMaximumTimeDuration_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::setProgressCallback
OT_SimulationAlgorithm_setProgressCallback_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::EventSimulation::setStopCallback
OT_SimulationAlgorithm_setStopCallback_doc
