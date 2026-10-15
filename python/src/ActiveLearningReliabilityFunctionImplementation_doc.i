%feature("docstring") OT::ActiveLearningReliabilityFunctionImplementation
"Base implementation of the active learning criterion for reliability analysis.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Use the handle :class:`~openturns.experimental.ActiveLearningReliabilityFunction`
to store any criterion. Concrete criteria
(:class:`~openturns.experimental.ActiveLearningUFunction`,
:class:`~openturns.experimental.ActiveLearningEFFFunction`,
:class:`~openturns.experimental.ActiveLearningGMMFunction`)
derive from this implementation.

See also
--------
ActiveLearningReliabilityFunction, ActiveLearningUFunction, ActiveLearningEFFFunction, ActiveLearningGMMFunction
"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunctionImplementation::setGaussianProcessRegression
"Gaussian Process Regressor accessor.

Parameters
----------
gprResult : GaussianProcessRegressionResult
    Gaussian Process Regressor Result
"
// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunctionImplementation::setReliabilityThreshold
"Reliability threshold accessor.

Parameters
----------
reliabilityThreshold : Scalar
    Threshold of the reliability event
"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunctionImplementation::getReliabilityThreshold
"Reliability threshold accessor.

Returns
-------
reliabilityThreshold : Scalar
    Threshold of the reliability event
"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunctionImplementation::getLearningThreshold
"Learning threshold accessor.

Returns
-------
learningThreshold : Scalar
    Threshold to determine if the learning process is converged.
"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunctionImplementation::getGaussianProcessRegression
"Gaussian Process Regressor accessor.

Returns
-------
gprResult : GaussianProcessRegressionResult
    Gaussian Process Regressor Result
"
// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunctionImplementation::setLearningThreshold
"Learning threshold accessor.

Parameters
----------
learningThreshold : Scalar
    Threshold to determine if the learning process is converged. This threshold can be used as termination criterion in active learning process.
"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningReliabilityFunctionImplementation::checkConvergenceLearning
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

%feature("docstring") OT::ActiveLearningReliabilityFunctionImplementation::getInfillSample
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

%feature("docstring") OT::ActiveLearningReliabilityFunctionImplementation::isMaximization
"Selection direction of the criterion.

Returns
-------
isMaximization : bool
    True if the infill point maximizes the criterion (EFF, GMM), False if it minimizes it (U).
"
