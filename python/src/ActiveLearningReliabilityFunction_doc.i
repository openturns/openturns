%feature("docstring") OT::ActiveLearningReliabilityFunction
"Active learning criterion for reliability analysis.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class is a base class for all the active learning criteria for reliability analysis.

The principle is to evaluate candidate samples on an active learning (also known as infill) criterion to identify the sample that is the most relevant for the metamodel update.

An :class:`~openturns.experimental.ActiveLearningReliabilityFunction` object can be created only through its derived classes,
:class:`~openturns.experimental.ActiveLearningUFunction`, :class:`~openturns.experimental.ActiveLearningEFFFunction` or :class:`~openturns.experimental.ActiveLearningGMMFunction`.

Parameters
----------
reliabilityThreshold : float
    Reliability analysis threshold.

learningThreshold : float
    Threshold used to check the active learning convergence.


See also
--------
ActiveLearningGMMFunction, ActiveLearningEFFFunction, ActiveLearningUFunction, ActiveLearningReliabilityAlgorithm
"

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

%feature("docstring") OT::ActiveLearningReliabilityFunction::getReliabilityThreshold
"Reliability threshold accessor.

Returns
-------
reliabilityThreshold : Scalar
    Threshold of the reliability event
"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunction::getLearningThreshold
"Learning threshold accessor.

Returns
-------
learningThreshold : Scalar
    Threshold to determine if the learning process is converged.
"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunction::getGaussianProcessRegression
"Gaussian Process Regressor accessor.

Returns
-------
gprResult : GaussianProcessRegressionResult
    Gaussian Process Regressor Result
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
criterionValues : :class:`~openturns.Sample`
    Values of the infill criterion on the candidate sample.

Returns
-------
convergenceIndicator : bool
    Indicator of convergence. The convergence is assumed if the infill values obtained for each candidate sample fulfill the convergence test (according to the learning threshold).
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityFunction::getInfillSample
"Select the infill sample to evaluate on the true function.

Parameters
----------
candidateSample : :class:`~openturns.Sample`
    Candidate sample to test.
criterionValues : :class:`~openturns.Sample`
    Values of infill criterion for the candidateSample.
Returns
-------
sampleToEvaluate : :class:`~openturns.Sample`
    Sample that maximize (or minimize) the active learning criterion. This sample will be evaluated on the true function.
"

// ---------------------------------------------------------------------------
%feature("docstring") OT::ActiveLearningReliabilityFunction::isMaximization
"Selection direction of the criterion.

Returns
-------
isMaximization : bool
    True if the infill point maximizes the criterion (EFF, GMM), False if it minimizes it (U).
"
        
