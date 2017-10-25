Latin Hypercube Simulation
--------------------------

| Let us note
  :math:`\cD_f = \{\ux \in \Rset^{n} \space | \space  g(\ux,\underline{d}) \leq 0\}`.
  The goal is to estimate the following probability:

  .. math::

     \begin{aligned}
         P_f  &=& \int_{\cD_f} f_{\uX}(\ux)d\ux\\
         &=& \int_{\Rset^{n}} \mathbf{1}_{\{g(\ux,\underline{d}) \leq 0 \}}f_{\uX}(\ux)d\ux\\
         &=& \Prob {\{\space g(\uX,\underline{d}) \leq 0 \}}
       \end{aligned}

| LHS or Latin Hypercube Sampling is a sampling method enabling to
  better cover the domain of variations of the input variables, thanks
  to a stratified sampling strategy. This method is applicable in the
  case of independent input variables. The sampling procedure is based
  on dividing the range of each variable into several intervals of equal
  probability. The sampling is undertaken as follows:

-  **Step 1**  The range of each input variable is stratified into
   isoprobabilistic cells,

-  **Step 2**  A cell is uniformly chosen among all the available cells,

-  **Step 3**  The random number is obtained by inverting the Cumulative
   Density Function locally in the chosen cell,

-  **Step 4**  All the cells having a common strate with the previous
   cell are put apart from the list of available cells.

| The estimator of the probability of failure with LHS is given by:

  .. math::

    \hat{P}_{f,LHS}^N = \frac{1}{N}\sum_{i=1}^N \mathbf{1}_{\{g(\uX^i,\underline{d}) \leq 0 \}}

  where the sample of :math:` \{ \uX^i,i=1 \hdots N \}` is obtained as
  described previously.

| One can show that:

  .. math::

    \Var{\hat{P}_{f,LHS}^N} \leq \frac{N}{N-1} . \Var{    \hat{P}_{f,MC}^N}

   where:

-  :math:`\Var {\hat{P}_{f,LHS}^N}` is the variance of the estimator of
   the probability of exceeding a threshold computed by the LHS
   technique,

-  :math:`\Var {\hat{P}_{f,MC}^N}` is the variance of the estimator of
   the probability of exceeding a threshold computed by a crude Monte
   Carlo method.

| With the notations

  .. math::

     \begin{aligned}
         \mu_N &=& \frac{1}{N}\sum_{i=1}^N \mathbf{1}_{\{g(\underline{x}_i),\underline{d}) \leq 0 \}}\\
         \sigma_N^2 &=& \frac{1}{N}\sum_{i=1}^N (\mathbf{1}_{\{g(\underline{x}^i),\underline{d}) \leq 0 \}} - \mu_N)^2
       \end{aligned}

the asymptotic confidence interval of order :math:`1-\alpha` associated
to the estimator :math:`P_{f,LHS}^N` is

.. math::

    [ \mu_N - \frac{q_{1-\alpha / 2} . \sigma_N}{\sqrt{N}} \space ; \space \mu_N + \frac{q_{1-\alpha / 2} . \sigma_N}{\sqrt{N}} ]

where :math:`q_{1-\alpha /2}` is the :math:`1-\alpha / 2` quantile from
the reduced standard gaussian law :math:`\cN(0,1)`.

It gives an unbiased estimate for :math:`P_f` (reminding that all input
variables must be independent).

This method is derived from a more general method called ’Stratified
Sampling’.


.. topic:: API:

    - See :class:`~openturns.LHS`


.. topic:: Examples:

    - See :ref:`examples/reliability_sensitivity/estimate_probability_lhs.ipynb`


.. topic:: References:

    - Mc Kay, Conover, Beckman, A comparison of three methods for selecting values of input variables in the analysis of output from a computer code, Technometrics, 21 (2), 1979
    - Latin Hypercube Sampling and the Propagation of Uncertainty in Analyses of Complex Systems, J. Helton, F.J. Davis, 2002, SAND 2001-0417
    - The Design and Analysis of Computer Experiments by Thomas J. Santner, Brian J. Williams, and William I. Notz, Springer Verlag, New York 2003
    - A Central Limit Theorem for Latin Hypercube Sampling, Art B. Owen, 1992, Journal of the Royal Statistical Society. Series B (Methodological), Vol. 54, No. 2, pp. 541-551
    - Large Sample Properties of Simulations Using Latin Hypercube Sampling, Michael Stein, Technometrics, Vol. 29, No. 2 (May, 1987), pp. 143-151

