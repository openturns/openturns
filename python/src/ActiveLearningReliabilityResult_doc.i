%feature("docstring") OT::ActiveLearningReliabilityResult
"Result of active learning reliability algorithm.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

It derives from :class:`~openturns.ProbabilitySimulationResult` so that
the failure probability estimate is visible through a generic
:class:`~openturns.EventSimulation` handle.

Per-iteration inner simulation results are kept with full dynamic type:
generic quantities read directly on
:class:`~openturns.GenericSimulationResult`, concrete details through
``getImplementation()``.

See also
--------
ActiveLearningReliabilityAlgorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getProbabilityEstimate
"Accessor to probability estimate

Returns
-------
probabilityEstimate : Scalar
   Probability estimation with active learning reliability algorithm
"
// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setProbabilityEstimate
"Accessor to probability estimate

Parameters
----------
probabilityEstimate : Scalar
   Probability estimation with active learning reliability algorithm
"


// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getReliabilityIndex
"Accessor to reliability index estimate

Returns
-------
reliabilityIndex : Scalar
   Reliability index estimation with active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setReliabilityIndex
"Accessor to reliability index estimate

Parameters
----------
reliabilityIndex : Scalar
   Reliability index estimation with active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getGprResult
"Accessor to Gaussian process result

Returns
-------
gprResult : :class:`~openturns.GaussianProcessRegressionResult`
   Result of Gaussian process regressor used in active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setGprResult
"Accessor to Gaussian process result

Parameters
----------
gprResult : :class:`~openturns.GaussianProcessRegressionResult`
   Result of Gaussian process regressor used in active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getProbabilityHistory
"Accessor to probability history

Returns
-------
probabilityHistory : sequence of float
   History of probability estimates during the iterations of active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setProbabilityHistory
"Accessor to probability history

Parameters
----------
probabilityHistory : sequence of float
   History of probability estimates during the iterations of active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getReliabilityIndexHistory
"Accessor to reliability index history

Returns
-------
reliabilityIndexHistory : sequence of float
   History of reliability index estimates during the iterations of active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setReliabilityIndexHistory
"Accessor to reliability index history

Parameters
----------
reliabilityIndexHistory : sequence of float
   History of reliability index estimates during the iterations of active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getFunctionCallNumber
"Accessor to function call number

Returns
-------
functionCallNumber : int
   Number of true function calls during the active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setFunctionCallNumber
"Accessor to function call number

Parameters
----------
functionCallNumber : int
   Number of true function calls during the active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setProbabilityConfidenceInterval
"Accessor to probability estimate confidence interval

Parameters
----------
probabilityCI : :class:`~openturns.Interval`
   Confidence interval of probability estimate, defined by `convergenceUncertaintyFactor`
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getProbabilityConfidenceInterval
"Accessor to probability estimate confidence interval

Returns
-------
probabilityCI : :class:`~openturns.Interval`
   Confidence interval of probability estimate, defined by `convergenceUncertaintyFactor`
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setReliabilityIndexConfidenceInterval
"Accessor to reliability index estimate confidence interval

Parameters
----------
reliabilityIndexCI : :class:`~openturns.Interval`
   Confidence interval of reliability index estimate, defined by `convergenceUncertaintyFactor`
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getReliabilityIndexConfidenceInterval
"Accessor to reliability index estimate confidence interval

Returns
-------
reliabilityIndexCI : :class:`~openturns.Interval`
   Confidence interval of reliability index estimate, defined by `convergenceUncertaintyFactor`
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getHasConverged
"Accessor to the convergence flag.

Returns
-------
hasConverged : bool
   True if the learning criterion converged within the iteration budget
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setHasConverged
"Accessor to the convergence flag.

Parameters
----------
hasConverged : bool
   True if the learning criterion converged within the iteration budget
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getSimulationResults
"Accessor to per-iteration inner simulation results.

Returns
-------
simulationResults : sequence of :class:`~openturns.GenericSimulationResult`
    Inner simulation result of each enrichment iteration, with full
    dynamic type. Generic quantities read directly; concrete details
    through conversion to the known concrete type.
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setSimulationResults
"Accessor to per-iteration inner simulation results.

Parameters
----------
simulationResults : sequence of :class:`~openturns.GenericSimulationResult`
    Inner simulation result of each enrichment iteration
"
