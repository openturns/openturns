.. _ranking_pcc:

Uncertainty ranking: PCC and PRCC
---------------------------------

Partial Correlation Coefficients
deal with analyzing the influence the random vector
:math:`\vect{X} = \left( X_1,\ldots,X_{n_X} \right)` has on a random
variable :math:`Y` which is being studied for uncertainty. Here we
attempt to measure linear relationships that exist between :math:`Y`
and the different components :math:`X_i`.

The basic method of hierarchical ordering using Pearson’s coefficients
deals with the case where the variable :math:`Y` linearly
depends on :math:`n_X` variables
:math:`\left\{ X_1,\ldots,X_{n_X} \right\}` but this can be misleading
when statistical dependencies or interactions between the variables
:math:`X_i` (e.g. a crossed term :math:`X_i \times X_j`) exist. In such
a situation, the partial correlation coefficients can be more useful in
ordering the uncertainty hierarchically: the partial correlation
coefficients :math:`\textrm{PCC}_{X_i,Y}` between the variables
:math:`Y` and :math:`X_i` attempts to measure the residual influence
of :math:`X_i` on :math:`Y` once influences from all other variables
:math:`X_j` have been eliminated.

The estimation for each partial correlation coefficient
:math:`\textrm{PCC}_{X_i,Y}` uses a set made up of :math:`N` values
:math:`\left\{ \left(y^{(1)},x_1^{(1)},\ldots,x_{n_X}^{(1)} \right),\ldots, \left(y^{(N)},x_1^{(N)},\ldots,x_{n_X}^{(N)} \right) \right\}`
of the vector :math:`(Y,X_1,\ldots,X_{n_X})`. This requires the
following three steps to be carried out:

#. Determine the effect of other variables
   :math:`\left\{ X_j,\ j\neq i \right\}` on :math:`Y` by linear
   regression; when the values of the variables
   :math:`\left\{ X_j,\ j\neq i \right\}` are known, the average
   forecast for the value of :math:`Y` is then available in the form
   of the equation:

   .. math::

      \begin{aligned}
            \widehat{Y} = \sum_{k \neq i,\ 1 \leq k \leq n_X} \widehat{a}_k X_k
          \end{aligned}

#. Determine the effect of other variables
   :math:`\left\{ X_j,\ j\neq i \right\}` on :math:`X_i` by linear
   regression; when the values of the variables
   :math:`\left\{ X_j,\ j\neq i \right\}` are known, the average
   forecast for the value of :math:`X_i` is then available in the form
   of the equation:

   .. math::

      \begin{aligned}
            \widehat{X}_i = \sum_{k \neq i,\ 1 \leq k \leq n_X} \widehat{b}_k X_k
          \end{aligned}

#. :math:`\textrm{PCC}_{X_i,Y}` is then equal to the Pearson
   correlation coefficient
   :math:`\widehat{\rho}_{Y-\widehat{Y},X_i-\widehat{X}_i}`
   estimated for the variables :math:`Y-\widehat{Y}` and
   :math:`X_i-\widehat{X}_i` on the :math:`N`-sample of simulations.

One can then class the :math:`n_X` variables :math:`X_1,\ldots, X_{n_X}`
according to the absolute value of the partial correlation coefficients:
the higher the value of :math:`\left| \textrm{PCC}_{X_i,Y} \right|`,
the greater the impact the variable :math:`X_i` has on :math:`Y`.

Partial *Rank* Correlation Coefficients (PRCC) are PRC coefficients
computed on the ranked input variables
:math:`r\vect{X} = \left( rX_1,\ldots,rX_{n_X} \right)`
and the ranked output variable :math:`rY`.


.. topic:: API:

    - See :meth:`~openturns.CorrelationAnalysis.computePCC`
    - See :meth:`~openturns.CorrelationAnalysis.computePRCC`


.. topic:: Examples:

    - See :doc:`/auto_data_analysis/manage_data_and_samples/plot_sample_correlation`


.. topic:: References:

    - [saltelli2000]_
    - [helton2003]_
    - [kleijnen1999]_
