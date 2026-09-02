%feature("docstring") OT::ActiveLearningEFFFunction
R"RAW(Active learning criterion for reliability analysis implementing the Expected Feasibility Function (EFF) function.

Parameters
----------
reliabilityThreshold : float
    Reliability analysis threshold.

learningThreshold : float
    Threshold used to check the active learning convergence.

See also
--------
ActiveLearningGMMFunction, ActiveLearningUFunction, ActiveLearningReliabilityFunction, ActiveLearningReliabilityAlgorithm

Notes
-----
This class inherits from :class:`~openturns.ActiveLearningReliabilityFunction` and implements the calculation of the Expected Feasibility Function (EFF)  for reliability analysis. The sample that maximizes the EFF will be selected for the refinement of the Gaussian Process Regressor.

Let consider :math:`\inputRV` a sample of dimension :math:`\inputDim` that is returned by a reliability simulation algorithm, :math:`T` the threshold defining the limit state of the reliability problem, and :math:`\widehat{\model}(\cdot)` and :math:`\widehat{\sigma}(\cdot)` respectively the mean and standard deviation of the conditioned Gaussian process used in the :class:`~openturns.ActiveLearningReliabilityAlgorithm`. The EFF function  :math:`EFF: \Rset^{\inputDim} \rightarrow \Rset` is defined as :

.. math::
   :nowrap:
   
   \begin{align*}
   EFF(\vect{x}) & =  (\widehat{\model}(\vect{x}) - T) \left[2 \Phi(\widehat{\model^{0}}(\vect{x}))-\Phi(\widehat{\model^{+}}(\vect{x}))-\Phi(\widehat{\model^{-}}(\vect{x}))\right]\\
                   & - \widehat{\sigma}(\vect{x}) \left[2 \phi(\widehat{\model^{0}}(\vect{x}))-\phi(\widehat{\model^{+}}(\vect{x}))-\phi(\widehat{\model^{-}}(\vect{x}))\right]\\
                   & + \epsilon(\vect{x})\left[ \Phi(\widehat{\model^{+}}(\vect{x}))-\Phi(\widehat{\model^{-}}(\vect{x}))\right]
   \end{align*}
   
with  :math:`\epsilon(\vect{x}) = 2 \widehat{\sigma}(\vect{x})`, :math:`\widehat{\model^{0}}(\vect{x}) = \frac{T-\widehat{\model}(\vect{x})}{\widehat{\sigma}(\vect{x})}`,
    :math:`\widehat{\model^{+}}(\vect{x})=\frac{T+\epsilon(\vect{x})-\widehat{\model}(\vect{x})}{\widehat{\sigma}(\vect{x})}` and :math:`\widehat{\model^{-}}(\vect{x})=\frac{T-\epsilon(\vect{x})-\widehat{\model}(\vect{x})}{\widehat{\sigma}(\vect{x})}`. :math:`\Phi(\cdot)` and :math:`\phi(\cdot)` are respectively the CDF and PDF of the Normal distribution.
)RAW"

