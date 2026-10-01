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
