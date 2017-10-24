Uncertainty ranking using PRCC
------------------------------

This method deals with analyzing the influence the random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)` has on the random
variable :math:`Y^j` which is being studied for uncertainty. Here we
attempt to measure monotonic relationships that exist between
:math:`Y^j` and the different components :math:`X^i`.

The basic method of hierarchical ordering using Spearman’s coefficients
deals with the case where the variable :math:`Y^j` monotonically
depends on :math:`n_X` variables
:math:`\left\{ X^1,\ldots,X^{n_X} \right\}` but this can be misleading
when statistical dependencies between the variables :math:`X^i` exist.
In such a situation, the partial rank correlation coefficients can be
more useful in ordering the uncertainty hierarchically: the partial rank
correlation coefficients :math:`\textrm{PRCC}_{X^i,Y^j}` between the
variables :math:`Y^j` and :math:`X^i` attempts to measure the residual
influence of :math:`X^i` on :math:`Y^j` once influences from all other
variables :math:`X^j` have been eliminated.

The estimation for each partial rank correlation coefficient
:math:`\textrm{PRCC}_{X^i,Y^j}` uses a set made up of :math:`N` values
:math:`\left\{ (y^j1,x_1^1,\ldots,x_1^{n_X}),\ldots,(y^jN,x_N^1,\ldots,x_N^{n_X}) \right\}`
of the vector :math:`(Y^j,X^1,\ldots,X^{n_X})`. This requires the
following three steps to be carried out:

#. Determine the effect of other variables
   :math:`\left\{ X^j,\ j\neq i \right\}` on :math:`Y^j` by linear
   regression (see ); when the values of variable
   :math:`\left\{ X^j,\ j\neq i \right\}` are known, the average
   forecast for the value of :math:`Y^j` is then available in the form
   of the equation:

   .. math::

    \widehat{Y^j} = \sum_{k \neq i,\ 1 \leq k \leq n_X} \widehat{a}_k X^k

#. Determine the effect of other variables
   :math:`\left\{ X^j,\ j\neq i \right\}` on :math:`X^i` by linear
   regression; when the values of variable
   :math:`\left\{ X^j,\ j\neq i \right\}` are known, the average
   forecast for the value of :math:`Y^j` is then available in the form
   of the equation:

   .. math::

    \widehat{X}^i = \sum_{k \neq i,\ 1 \leq k \leq n_X} \widehat{b}_k X^k

#. :math:`\textrm{PRCC}_{X^i,Y^j}` is then equal to the Spearman’s
   correlation coefficient
   :math:`\widehat{\rho}^S_{Y^j-\widehat{Y^j},X^i-\widehat{X}^i}`
   estimated for the variables :math:`Y^j-\widehat{Y^j}` and
   :math:`X^i-\widehat{X}^i` on the :math:`N`-sample of simulations (see
   ).

One can then class the :math:`n_X` variables :math:`X^1,\ldots, X^{n_X}`
according to the absolute value of the partial rank correlation
coefficients: the higher the value of
:math:`\left| \textrm{PRCC}_{X^i,Y^j} \right|`, the greater the impact
the variable :math:`X^i` has on :math:`Y^j`.


.. topic:: API:

    - See :py:func:`~openturns.CorrelationAnalysis_PRCC`


.. topic:: Examples:

    - See :ref:`examples/data_analysis/sample_correlation.ipynb`


.. topic:: References:

    - Saltelli, A., Chan, K., Scott, M. (2000). "Sensitivity Analysis", John Wiley \& Sons publishers, Probability and Statistics series
    - J.C. Helton, F.J. Davis (2003). "Latin Hypercube sampling and the propagation of uncertainty analyses of complex systems". Reliability Engineering and System Safety 81, p.23-69
    - J.P.C. Kleijnen, J.C. Helton (1999). "Statistical analyses of scatterplots to identify factors in large-scale simulations, part 1 : review and comparison of techniques". Reliability Engineering and System Safety 65, p.147-185

