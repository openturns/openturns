.. _ranking_pcc:

Uncertainty ranking: PCC and PRCC
---------------------------------

Partial Correlation Coefficients
deal with analyzing the influence the random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)` has on a random
variable :math:`Y` which is being studied for uncertainty. Here we
attempt to measure linear relationships that exist between :math:`Y`
and the different components :math:`X^i`.

The basic method of hierarchical ordering using Pearson’s coefficients
deals with the case where the variable :math:`Y` linearly
depends on :math:`n_X` variables
:math:`\left\{ X^1,\ldots,X^{n_X} \right\}` but this can be misleading
when statistical dependencies or interactions between the variables
:math:`X^i` (e.g. a crossed term :math:`X^i \times X^j`) exist. In such
a situation, the partial correlation coefficients can be more useful in
ordering the uncertainty hierarchically: the partial correlation
coefficients :math:`\textrm{PCC}_{X^i,Y}` between the variables
:math:`Y` and :math:`X^i` attempts to measure the residual influence
of :math:`X^i` on :math:`Y` once influences from all other variables
:math:`X^j` have been eliminated.

The estimation for each partial correlation coefficient
:math:`\textrm{PCC}_{X^i,Y}` uses a set made up of :math:`N` values
:math:`\left\{ (Y_1,x_1^1,\ldots,x_1^{n_X}),\ldots,(Y_N,x_N^1,\ldots,x_N^{n_X}) \right\}`
of the vector :math:`(Y,X^1,\ldots,X^{n_X})`. This requires the
following three steps to be carried out:

#. Determine the effect of other variables
   :math:`\left\{ X^j,\ j\neq i \right\}` on :math:`Y` by linear
   regression; when the values of the variables
   :math:`\left\{ X^j,\ j\neq i \right\}` are known, the average
   forecast for the value of :math:`Y` is then available in the form
   of the equation:

   .. math::

      \begin{aligned}
            \widehat{Y} = \sum_{k \neq i,\ 1 \leq k \leq n_X} \widehat{a}_k X^k
          \end{aligned}

#. Determine the effect of other variables
   :math:`\left\{ X^j,\ j\neq i \right\}` on :math:`X^i` by linear
   regression; when the values of the variables
   :math:`\left\{ X^j,\ j\neq i \right\}` are known, the average
   forecast for the value of :math:`X^i` is then available in the form
   of the equation:

   .. math::

      \begin{aligned}
            \widehat{X}^i = \sum_{k \neq i,\ 1 \leq k \leq n_X} \widehat{b}_k X^k
          \end{aligned}

#. :math:`\textrm{PCC}_{X^i,Y}` is then equal to the Pearson
   correlation coefficient
   :math:`\widehat{\rho}_{Y-\widehat{Y},X^i-\widehat{X}^i}`
   estimated for the variables :math:`Y-\widehat{Y}` and
   :math:`X^i-\widehat{X}^i` on the :math:`N`-sample of simulations.

One can then class the :math:`n_X` variables :math:`X^1,\ldots, X^{n_X}`
according to the absolute value of the partial correlation coefficients:
the higher the value of :math:`\left| \textrm{PCC}_{X^i,Y} \right|`,
the greater the impact the variable :math:`X^i` has on :math:`Y`.

Partial *Rank* Correlation Coefficients (PRCC) are PRC coefficients
computed on the ranked input variables
:math:`r\vect{X} = \left( rX^1,\ldots,rX^{n_X} \right)`
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
