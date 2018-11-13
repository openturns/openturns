.. _ranking_srrc:

Uncertainty ranking: SRRC
-------------------------

This method deals with analysing the influence the random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)` has on a random
variable :math:`Y^j` which is being studied for uncertainty. Here we
attempt to measure monotonic relationships that exist between :math:`Y^j`
and the different components :math:`X^i`.

The basic method of hierarchical ordering using Spearman’s coefficients
deals with the case where the variable :math:`Y^j` monotonically
depends on :math:`n_X` variables
:math:`\left\{ X^1,\ldots,X^{n_X} \right\}`.

In such a situation, the standard rank correlation coefficients can be
more useful in ordering the uncertainty hierarchically: the correlation
coefficients :math:`\textrm{SRCC}_{X^i,Y^j}` between the
variables :math:`Y^j` and :math:`X^i` attempts to measure the linear influence
of :math:`rX^i` has on :math:`rY^j` where :math:`rX^i` (respectively :math:`rY^j`)
is the ranked i-th input variable (respectively the ranked output variable).
The coefficients are measured using a linear regression model that links
the variable :math:`rY^j` to the :math:`n_x` variables :math:`rX^1,\ldots,rX^{n_X}`:

.. math::

    rY^j = a_0^j + \sum_{i=1}^{n_X} a_i^j rX^i + \varepsilon^j

:math:`\varepsilon^j` describes a random variable with zero mean and standard
deviation :math:`\sigma_{\varepsilon}^j` independent of the input variables :math:`rX^i`.
If the random variables :math:`rX^1,\ldots,rX^{n_X}` are independent and with finite variance
:math:`\Var{rX^k} = (\sigma_k)^2`, the variance of :math:`rY^j` can be
estimated as follows:

.. math::

    \Var{rY^j} = \sum_{i=1}^n (a_i^j)^2 \Var{rX^i}

The estimators for the regression coefficients
:math:`a_0^j,\ldots,a_{n_X}^j`, and the standard deviation
:math:`\sigma^j` are obtained from a sample of :math:`(Y^j,X^1,\ldots,X^{n_X})`.
Uncertainty ranking by linear regression ranks the :math:`n_X` variables
:math:`rX^1,\ldots, rX^{n_X}` in terms of the estimated contribution of each
:math:`rX^k` to the variance of :math:`Y^j`:

.. math::

    C^j_k = \frac{\displaystyle (a_k^j)^2  \Var{rX^k}}{\Var{Y^j}}


which is estimated by:

.. math::

    \widehat{C}^j_k = \frac{\displaystyle (\widehat{a}_k^j)^2 \widehat{\sigma}_k^2}{\displaystyle \sum_{i=1}^{n_X} (a_i^j)^2 \widehat{\sigma}_i^2}


where :math:`\widehat{\sigma}_i` describes the empirical standard deviation
of the sample of the input variables. As we consider rank marginals, the marginal
standard deviation is the same for all the input/output variables. Thus estimation simplifies:

.. math::

    \widehat{C}^j_k = \frac{\displaystyle (\widehat{a}_k^j)^2}{\displaystyle \sum_{i=1}^{n_X} (a_i^j)^2 }


This estimated contribution is by definition between 0 and 1. The closer it is to 1,
the greater the impact the variable :math:`X^i` has on the dispersion of :math:`Y^j`.

The contribution to the variance :math:`C_i` is sometimes described in
the literature as the “importance factor”, because of the similarity
between this approach to linear regression and the method of cumulative
variance quadratic which uses the term importance factor.


.. topic:: API:

    - See :py:func:`~openturns.CorrelationAnalysis_SRRC`


.. topic:: Examples:

    - See :doc:`/examples/data_analysis/sample_correlation`


.. topic:: References:

    - Saltelli, A., Chan, K., Scott, M. (2000). "Sensitivity Analysis", John Wiley \& Sons publishers, Probability and Statistics series
    - J.C. Helton, F.J. Davis (2003). "Latin Hypercube sampling and the propagation of uncertainty analyses of complex systems". Reliability Engineering and System Safety 81, p.23-69
    - J.P.C. Kleijnen, J.C. Helton (1999). "Statistical analyses of scatterplots to identify factors in large-scale simulations, part 1 : review and comparison of techniques". Reliability Engineering and System Safety 65, p.147-185

