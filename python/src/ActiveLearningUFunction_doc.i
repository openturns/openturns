%feature("docstring") OT::ActiveLearningUFunction
R"RAW(Active learning criterion for reliability analysis implementing the "U" function.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class inherits from :class:`~openturns.experimental.ActiveLearningReliabilityFunction` and implements the calculation of the "U" function for reliability analysis. The sample that minimizes the "U" function will be selected for the refinement of the Gaussian Process Regressor.

Consider :math:`\inputRV` a sample of dimension :math:`\inputDim` that is returned by a reliability simulation algorithm, :math:`T` the threshold defining the limit state of the reliability problem, and :math:`\widehat{\model}(\cdot)` and :math:`\widehat{\sigma}(\cdot)` respectively the mean and standard deviation of the conditioned Gaussian process used in the :class:`~openturns.experimental.ActiveLearningReliabilityAlgorithm`. The "U" function  :math:`U: \Rset^{\inputDim} \rightarrow \Rset^{+}` is defined as :

.. math::

   U(\vect{x}) = \frac{ \left| T - \widehat{\model}( \vect{x} ) \right| }{\widehat{\sigma}( \vect{x} )}

See [echard2011]_.

Parameters
----------
reliabilityThreshold : float
    Reliability analysis threshold.

learningThreshold : float
    Threshold used to check the active learning convergence.


See also
--------
ActiveLearningGMMFunction, ActiveLearningEFFFunction, ActiveLearningReliabilityFunction, ActiveLearningReliabilityAlgorithm
)RAW"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningUFunction::checkConvergenceLearning
"Test convergence of the U learning criterion. Convergence holds when the
minimum U value over the candidate sample reaches the learning threshold.

Parameters
----------
criterionValues : :class:`~openturns.Sample`
    Values of the U criterion on the candidate sample.

Returns
-------
convergenceIndicator : bool
    True if learning has converged.
"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningUFunction::getInfillSample
"Select the candidate sample minimizing the U criterion.

Parameters
----------
candidateSample : :class:`~openturns.Sample`
    Candidate sample to test.
criterionValues : :class:`~openturns.Sample`
    Values of the U criterion for the candidateSample.

Returns
-------
sampleToEvaluate : :class:`~openturns.Sample`
    Candidate sample with the smallest U value, to evaluate on the true function.
"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ActiveLearningUFunction::isMaximization
"Selection direction of the U criterion.

Returns
-------
isMaximization : bool
    Always False: the infill point minimizes U.
"
