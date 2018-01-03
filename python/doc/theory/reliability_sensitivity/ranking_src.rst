Uncertainty ranking: SRC
------------------------

This method deals with analysing the influence the random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)` has on a random
variable :math:`Y^j` which is being studied for uncertainty. Here we
attempt to measure linear relationships that exist between :math:`Y^j`
and the different components :math:`X^i`.

The principle of the multiple linear regression model (see for more
details) consists of attempting to find the function that links the
variable :math:`Y^j` to the :math:`n_x` variables
:math:`X^1,\ldots,X^{n_X}` by means of a linear model:

.. math::

    Y^j = a_0^j + \sum_{i=1}^{n_X} a_i^j X^i + \varepsilon^j

where :math:`\varepsilon^j` describes a random variable with zero mean
and standard deviation :math:`\sigma_{\varepsilon}^j` independent of the
input variables :math:`X^i`. If the random variables
:math:`X^1,\ldots,X^{n_X}` are independent and with finite variance
:math:`\Var{X^k} = (\sigma_k)^2`, the variance of :math:`Y^j` can be
written as follows:

.. math::

    \Var{Y^j} = \sum_{i=1}^n (a_i^j)^2 \Var{X^i} + (\sigma_{\varepsilon}^j)^2

The estimators for the regression coefficients
:math:`a_0^j,\ldots,a_{n_X}^j`, and the standard deviation
:math:`\sigma^j` are obtained from a sample of
:math:`(Y^j,X^1,\ldots,X^{n_X})`. Uncertainty ranking by linear
regression ranks the :math:`n_X` variables :math:`X^1,\ldots, X^{n_X}`
in terms of the estimated contribution of each :math:`X^k` to the
variance of :math:`Y^j`:

.. math::

    C^j_k = \frac{\displaystyle (a_k^j)^2  \Var{X^k}}{ \Var{Y^j}}


which is estimated by:

.. math::

    \widehat{C}^j_k = \frac{\displaystyle (\widehat{a}_k^j)^2 \widehat{\sigma}_k^2}{\displaystyle \sum_{i=1}^{n_X} (a_i^j)^2 \widehat{\sigma}_i^2 + (\widehat{\sigma}_{\varepsilon}^j)^2}


where :math:`\widehat{\sigma}_i` describes the empirical standard
deviation of the sample of the input variables. This estimated
contribution is by definition between 0 and 1. The closer it is to 1,
the greater the impact the variable :math:`X^i` has on the dispersion of
:math:`Y^j`.

The contribution to the variance :math:`C_i` is sometimes described in
the literature as the “importance factor”, because of the similarity
between this approach to linear regression and the method of cumulative
variance quadratic which uses the term importance factor.


.. topic:: API:

    - See :py:func:`~openturns.CorrelationAnalysis_SRC`


.. topic:: Examples:

    - See :doc:`/examples/data_analysis/sample_correlation`


.. topic:: References:

    - Saltelli, A., Chan, K., Scott, M. (2000). "Sensitivity Analysis", John Wiley \& Sons publishers, Probability and Statistics series
    - J.C. Helton, F.J. Davis (2003). "Latin Hypercube sampling and the propagation of uncertainty analyses of complex systems". Reliability Engineering and System Safety 81, p.23-69
    - J.P.C. Kleijnen, J.C. Helton (1999). "Statistical analyses of scatterplots to identify factors in large-scale simulations, part 1 : review and comparison of techniques". Reliability Engineering and System Safety 65, p.147-185

