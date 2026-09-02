%feature("docstring") OT::ActiveLearningUFunction
R"RAW(Active learning criterion for reliability analysis implementing the "U" function.

Parameters
----------
reliabilityThreshold : float
    Reliability analysis threshold.

learningThreshold : float
    Threshold used to check the active learning convergence.


See also
--------
ActiveLearningGMMFunction, ActiveLearningEFFFunction, ActiveLearningReliabilityFunction, ActiveLearningReliabilityAlgorithm

Notes
-----
This class inherits from :class:`~openturns.ActiveLearningReliabilityFunction` and implements the calculation of the "U" function for reliability analysis. The sample that minimizes the "U" function will be selected for the refinement of the Gaussian Process Regressor.

Let consider :math:`\inputRV` a sample of dimension :math:`\inputDim` that is returned by a reliability simulation algorithm, :math:`T` the threshold defining the limit state of the reliability problem, and :math:`\widehat{\model}(\cdot)` and :math:`\widehat{\sigma}(\cdot)` respectively the mean and standard deviation of the conditioned Gaussian process used in the :class:`~openturns.ActiveLearningReliabilityAlgorithm`. The "U" function  :math:`U: \Rset^{\inputDim} \rightarrow \Rset^{+}` is defined as :

.. math::

   U(\vect{x}) = \frac{ \left| T - \widehat{\model}( \vect{x} ) \right| }{\widehat{\sigma}( \vect{x} )}

)RAW"

