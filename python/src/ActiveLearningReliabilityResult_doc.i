%feature("docstring") OT::ActiveLearningReliabilityResult
"Result of active learning reliability algorithm.

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
probabilityHistory : ScalarCollection
   History of probability estimates during the iterations of active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setProbabilityHistory
"Accessor to probability history

Parameters
----------
probabilityHistory : ScalarCollection
   History of probability estimates during the iterations of active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getReliabilityIndexHistory
"Accessor to reliability index history

Returns
-------
reliabilityIndexHistory : ScalarCollection
   History of reliability index estimates during the iterations of active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setReliabilityIndexHistory
"Accessor to reliability index history

Parameters
----------
reliabilityIndexHistory : ScalarCollection
   History of reliability index estimates during the iterations of active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getFunctionCallNumber
"Accessor to function call number

Parameters
----------
functionCallNumber : Scalar
   Number of true function calls during the active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setFunctionCallNumber
"Accessor to function call number

Returns
-------
functionCallNumber : Scalar
   Number of true function calls during the active learning reliability algorithm
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setProbabilityConfidenceInterval
"Accessor to probability estimate confidence interval

Returns
-------
probabilityCI : :class:`~openturns.Interval`
   Confidence index interval  of probability estimate, defined by `convergenceUncertaintyFactor`
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getProbabilityConfidenceInterval
"Accessor to probability estimate confidence interval

Parameters
----------
probabilityCI : :class:`~openturns.Interval`
   Confidence index interval  of probability estimate, defined by `convergenceUncertaintyFactor`
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::setReliabilityIndexConfidenceInterval
"Accessor to reliability index estimate confidence interval

Returns
-------
reliabilityIndexCI : :class:`~openturns.Interval`
   Confidence index interval of reliability index estimate, defined by `convergenceUncertaintyFactor`
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityResult::getReliabilityIndexConfidenceInterval
"Accessor to reliability index estimate confidence interval

Parameters
----------
reliabilityIndexCI : :class:`~openturns.Interval`
   Confidence index interval of reliability index estimate, defined by `convergenceUncertaintyFactor`
"
