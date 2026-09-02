%feature("docstring") OT::ActiveLearningReliabilityFunction
"Active learning criterion for reliability analysis.

Parameters
----------
reliabilityThreshold : float
    Reliability analysis threshold.

learningThreshold : float
    Threshold used to check the active learning convergence.


See also
--------
ActiveLearningGMMFunction, ActiveLearningEFFFunction, ActiveLearningUFunction, ActiveLearningReliabilityAlgorithm

Notes
-----
This class is a base class for all the active learning criteria for reliability analysis.

The principle is to evaluate candidate samples on an active learning (also known as infill) criterion to identify the sample that is the most relevant for the metamodel update.

An :class:`~openturns.ActiveLearningReliabilityFunction` object can be created only through its derivative classes,
:class:`~openturns.ActiveLearningUFunction`, :class:`~openturns.ActiveLearningEFFFunction` or :class:`~openturns.ActiveLearningGMMFunction`."

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunction::setGaussianProcessRegression
"Gaussian Process Regressor accessor.

Parameters
----------
gprResult : GaussianProcessRegressionResult
    Gaussian Process Regressor Result
"
// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunction::setReliabilityThreshold
"Reliability threshold accessor.

Parameters
----------
reliabilityThreshold : Scalar
    Threshold of the reliability event
"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunction::setLearningThreshold
"Learning threshold accessor.

Parameters
----------
learningThreshold : Scalar
    Threshold to determine if the learning process is converged. This threshold can be used as termination criterion in active learning process.
"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunction::checkConvergenceLearning
"Test convergence of active learning.

Parameters
----------
candidateSample : :class:`~openturns.Sample`
    Candidate sample to test active learning is converged.
    
Returns
-------
convergenceIndicator : Bool
    Indicator of convergence. The convergence is assumed if the infill values obtained for each candidate sample fulfill the convergence test (according to the learning threshold).
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityFunction::getInfillSample
"Learning threshold accessor.

Parameters
----------
candidateSample : :class:`~openturns.Sample`
    Candidate sample to test.
criterionValues  :class:`~openturns.Sample`
    Values of infill criterion for the candidateSample.
Returns
-------
sampleToEvaluate : :class:`~openturns.Sample`
    Sample that maximize (or minimize) the active learning criterion. This sample will be evaluated on the true function.
"
        
