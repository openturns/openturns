.. _qmc_simulation:

Quasi Monte Carlo
-----------------

| Let us note
  :math:`\cD_f = \{\ux \in \Rset^{\inputDim} \: | \:  \model(\ux) \leq 0\}`.
  The goal is to estimate the following probability:

  .. math::
    :label: integ

     \begin{aligned}
         P_f &= \int_{\cD_f} f_{\uX}(\ux)d\ux\\
         &= \int_{\Rset{\inputDim}} \mathbf{1}_{\{\model(\ux) \leq 0 \}}f_{\uX}(\ux)d\ux\\
         &= \Prob {\{\:\model(\uX) \leq 0 \}}
       \end{aligned}

| Quasi-Monte Carlo is a technique which approximates the integral
  :eq:`integ` using low discrepancy sequences
  :math:`\{\vect{x}^1, ..., \vect{x}^\sampleSize\}` instead of randomly generated
  sequences, as follows:

  .. math::

    P_f \approx \frac{1}{\sampleSize}\,\sum_{i=1}^\sampleSize  \mathbf{1}_{\cD_f}(\ux_i) f(\ux_i).

| In general, the integral of a function :math:`f` on
  :math:`\Delta = [0,1]^s` can be approximated by using some low
  discrepancy sequence :math:`\{\vect{x}_1, \hdots, \vect{x}_\sampleSize\}` as
  follows:

  .. math::

     \int_{\Delta} f(\vect{u})\,d\vect{u} \approx \frac{1}{\sampleSize}\,\sum_{i=1}^\sampleSize f(\vect{x}_i).

The low discrepancy sequence is generated on :math:`\Delta` according to
the Lebesgue measure then may be transformed to any measure :math:`\mu`
thanks to the inverse CDF technique in order to approximate the integral
:

.. math::

   \begin{aligned}
       \int_{\Rset^s} f(\vect{u})\,d\vect{u} \approx \frac{1}{\sampleSize}\,\sum_{i=1}^\sampleSize f(\vect{x}_i).
     \end{aligned}

As the sequence is not randomly generated, we can not use the Central
Limit Theorem in order to control the quality of the approximation. This
quality is given by the Koksma-Hlawka inequality that we recall here :

.. math::

   \begin{aligned}
       \left\lvert \frac{1}{\sampleSize}\sum_{i=1}^\sampleSize f(\vect{x}_i) - \int_If(\vect{x})d\vect{x} \right\rvert \le Var(f)D^\sampleSize(\vect{x}_1, ..., \vect{x}_\sampleSize)
     \end{aligned}

where :math:`D^\sampleSize(\vect{x}_1, ..., \vect{x}_\sampleSize)` is the discrepancy of
the sequence :math:`\{\vect{x}_1, \hdots, \vect{x}_\sampleSize\}`.

| For Halton, Inverse Halton and Sobol sequences, we have :

  .. math::

     \begin{aligned}
         D^\sampleSize = O\biggl(\frac{\log^s{\sampleSize}}{\sampleSize}\biggr)
       \end{aligned}

| Thus, asymptotically the error converges in
  :math:`O\biggl(\frac{\log^s{\sampleSize}}{\sampleSize}\biggr)`, instead of
  :math:`O\biggl(\frac{1}{\sqrt{\sampleSize}}\biggr)` for traditional Monte Carlo.
  The convergence rate depends on the dimension :math:`s` so one must
  have :math:`\sampleSize >> e^s`.


.. topic:: API:

    - See :class:`~openturns.ProbabilitySimulationAlgorithm`
    - See :class:`~openturns.LowDiscrepancyExperiment`


.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/reliability/plot_estimate_probability_randomized_qmc`
