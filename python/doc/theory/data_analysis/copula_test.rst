.. _copula_test:

Goodness-of-fit tests for copulas
---------------------------------

The Cramer-von Mises and Kolmogorov-Smirnov tests below check whether a
multivariate sample is drawn from a given copula. They compare the
empirical copula built from the sample with the candidate copula, following
the approach of [genest2009]_.

Let :math:`\vect{X}_1, \dots, \vect{X}_{\sampleSize}` be a sample of
dimension :math:`\inputDim \geq 2` with continuous margins and copula :math:`C`. Since copulas are
invariant under strictly increasing transformations of the margins, the
tests work on the pseudo-observations

.. math::

    \hat{\vect{U}}_i = \left( \frac{R_{i,1} + 1}{\sampleSize + 1}, \dots,
        \frac{R_{i,\inputDim} + 1}{\sampleSize + 1} \right), \quad i = 1, \dots, \sampleSize,

where :math:`R_{i,j}` is the rank of :math:`X_{i,j}` among
:math:`X_{1,j}, \dots, X_{\sampleSize,j}`. The sample under test can
therefore be either a sample from the copula itself (values in
:math:`[0, 1]^\inputDim`) or a raw observations sample: the pseudo-observations
are computed internally from the ranks. Let :math:`C_{\sampleSize}` be the
empirical cumulative distribution function of the unknown copula :math:`C`,
defined by:

.. math::

    C_{\sampleSize}(\vect{u}) = \frac{1}{\sampleSize} \sum_{i=1}^{\sampleSize}
        \prod_{j=1}^\inputDim 1_{\hat{U}_{i,j} \leq u_j}, \quad \vect{u} \in [0, 1]^\inputDim.

Let :math:`C_{\vect{\theta}_{\sampleSize}}` be the candidate copula: either a fully
specified copula, or a copula whose parameters :math:`\vect{\theta}_{\sampleSize}`
have been estimated from the sample with a
:class:`~openturns.DistributionFactory` (the estimated distribution must be a copula).
Two statistics measure the distance
between :math:`C_{\sampleSize}` and :math:`C_{\vect{\theta}_{\sampleSize}}`:

- the Cramer-von Mises statistic

  .. math::

      S_{\sampleSize} = \sum_{i=1}^{\sampleSize} \left[
          C_{\sampleSize}(\hat{\vect{U}}_i) -
          C_{\vect{\theta}_{\sampleSize}}(\hat{\vect{U}}_i) \right]^2,

- the Kolmogorov-Smirnov statistic

  .. math::

      T_{\sampleSize} = \max_{i=1, \dots, \sampleSize} \left|
          C_{\sampleSize}(\hat{\vect{U}}_i) -
          C_{\vect{\theta}_{\sampleSize}}(\hat{\vect{U}}_i) \right|.

Large values of these statistics lead to rejection of the null hypothesis
:math:`\mathcal{H}_0 = \{ C \in \mathcal{C}_0 \}` that the underlying copula
belongs to the candidate family. Under :math:`\mathcal{H}_0`, the asymptotic
distributions of :math:`S_{\sampleSize}` and :math:`T_{\sampleSize}` depend
on the unknown copula parameter, so they cannot be tabulated: the p-value
is estimated with a parametric bootstrap. Bootstrap samples are drawn from
the tested (or estimated) copula and the statistic is recomputed on each of
them, re-estimating the parameters in the factory case, until the requested
precision is reached. This follows the same adaptive Monte Carlo scheme as
the :ref:`kolmogorov_smirnov_test` with estimated parameters (Lilliefors
test).

We fix a risk :math:`\alpha` (type I error) and reject the candidate copula
if the estimated p-value is lower than or equal to :math:`\alpha`.


.. topic:: API:

    - See :py:func:`~openturns.FittingTest.CopulaCramerVonMises`
    - See :py:func:`~openturns.FittingTest.CopulaKolmogorov`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/statistical_tests/plot_fitted_copula_ranking` for ranking copulas by BIC and goodness-of-fit tests
    - See :doc:`/auto_data_analysis/statistical_tests/plot_test_copula` for a graphical validation of a copula

.. topic:: References:

    - [genest2009]_
