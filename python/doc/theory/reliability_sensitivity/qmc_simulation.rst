Quasi Monte Carlo
-----------------

| Let us note
  :math:`\cD_f = \{\ux \in \Rset^{n} \: | \:  g(\ux,\underline{d}) \leq 0\}`.
  The goal is to estimate the following probability:

  .. math::
    :label: integ

     \begin{aligned}
         P_f &=& \int_{\cD_f} f_{\uX}(\ux)d\ux\\
         &=& \int_{\Rset^{n}} \mathbf{1}_{\{g(\ux,\underline{d}) \leq 0 \}}f_{\uX}(\ux)d\ux\\
         &=& \Prob {\{\:g(\uX,\underline{d}) \leq 0 \}}
       \end{aligned}

| Quasi-Monte Carlo is a technique which approximates the integral
  :eq:`integ` using low discrepancy sequences
  :math:`\{\vect{x}^1, ..., \vect{x}^N\}` instead of randomly generated
  sequences, as follows:

  .. math::

    P_f \approx \frac{1}{N}\,\sum_{i=1}^N  \mathbf{1}_{\cD_f}(\ux_i) f(\ux_i).

| In general, the integral of a function :math:`f` on
  :math:`\Delta = [0,1]^s` can be approximated by using some low
  discrepancy sequence :math:`\{\vect{x}_1, \hdots, \vect{x}_N\}` as
  follows:

  .. math::

     \int_{\Delta} f(\vect{u})\,d\vect{u} \approx \frac{1}{N}\,\sum_{i=1}^N f(\vect{x}_i).

The low discrepancy sequence is generated on :math:`\Delta` according to
the Lebesgue measure then may be transformed to any measure :math:`\mu`
thanks to the inverse CDF technique in order to approximate the integral
:

.. math::

   \begin{aligned}
       \int_{\Rset^s} f(\vect{u})\,d\vect{u} \approx \frac{1}{N}\,\sum_{i=1}^N f(\vect{x}_i).
     \end{aligned}

As the sequence is not randomly generated, we can not use the Central
Limit Theorem in order to control the quality of the approximation. This
quality is given by the Koksma-Hlawka inequality that we recall here :

.. math::

   \begin{aligned}
       \left\lvert \frac{1}{N}\sum_{i=1}^Nf(\vect{x}_i) - \int_If(\vect{x})d\vect{x} \right\rvert \le Var(f)D^N(\vect{x}_1, ..., \vect{x}_N)
     \end{aligned}

where :math:`D^N(\vect{x}_1, ..., \vect{x}_N)` is the discrepancy of
the sequence :math:`\{\vect{x}_1, \hdots, \vect{x}_N\}`.

| For Halton, Inverse Halton and Sobol sequences, we have :

  .. math::

     \begin{aligned}
         D^N = O\biggl(\frac{\log^s{N}}{N}\biggr)
       \end{aligned}

| Thus, asymptotically the error converges in
  :math:`O\biggl(\frac{\log^s{N}}{N}\biggr)`, instead of
  :math:`O\biggl(\frac{1}{\sqrt{N}}\biggr)` for traditional Monte Carlo.
  The convergence rate depends on the dimension :math:`s` so one must
  have :math:`N >> e^s`.


.. topic:: API:

    - See :class:`~openturns.ProbabilitySimulationAlgorithm`
    - See :class:`~openturns.LowDiscrepancyExperiment`


.. topic:: Examples:

    - See :doc:`/examples/reliability_sensitivity/estimate_probability_randomized_qmc`
