.. _aic:

Akaike Information Criterion (AIC)
----------------------------------

This method can be used to rank candidate distributions with respect to data
:math:`\left\{ \inputReal_1,\inputReal_2,\ldots,\inputReal_{\sampleSize} \right\}`.

We denote by :math:`\cM_1, \dots, \cM_K` the :math:`K` parametric models we want to test
(see :ref:`parametric models <parametric_distribution_factories>`).
We suppose here that the parameters of these models have been estimated from the sample (see
:ref:`Maximum Likelihood <maximum_likelihood>`). We
denote by :math:`L_i` the maximized likelihood of the sample with respect to
the model :math:`\cM_i`.

By definition of the likelihood, the higher :math:`L_i`, the better the
model describes the sample. However, by relying entirely on the value of the likelihood one
runs the risk of systematically selecting the model with the most parameters. As a matter of fact,
the greater the number of parameters, the easier it is for the distribution to adapt to the data.

The Akaike Information Criterion (AIC) can be used to avoid this problem. Like the Bayesian information
criterion (see :ref:`bic`), it allows models to be penalized according to the number of parameters, in order
to satisfy the parsimony criterion. Note that the library divides the AIC
defined in the literature by the sample size, which has no impact on the selection of the best model.

The AIC of the model :math:`\cM_i` is defined in the library by:

.. math::

   \begin{aligned}
       \operatorname{AIC}(\cM_i) = -2 \frac{\log(L_i)}{\sampleSize} + \frac{2 k_i}{\sampleSize}
     \end{aligned}

where :math:`k_i` denotes the number of parameters of the model :math:`\cM_i`
that have been inferred from the sample.

The smaller :math:`\textrm{AIC}(\cM_i)`, the better
the model:

.. math::

   \cM_{AIC} = \argmin_{\cM_i, 1\leq i \leq K} {AIC}(\cM_i)

The idea is to introduce a penalization term that
increases with the numbers of parameters to be estimated. A complex
model will then have a good score only if the gain in terms of
likelihood is high enough to justify the number of parameters used.

In context of small data, there is a substantial risk that AIC still might systematically
select the models that have the most parameters. In other words, the risk of
overfitting is important. To tackle such an issue, the *corrected* AIC  criterion (AICc) was
developed: it consists in evaluating the AIC with a correction term
(extra penalty) for small data. The AICc of the model :math:`\cM_i` is defined by:

.. math::

   \begin{aligned}
       \operatorname{AICc}(\cM_i) = \operatorname{AIC}(\cM_i) + \frac{(2 k_i)(k_i + 1)}{\sampleSize - k_i - 1}
     \end{aligned}

One should notice that the extra term penalty vanishes for
:math:`\sampleSize \rightarrow \infty`.


.. topic:: API:

    - See :py:meth:`~openturns.FittingTest.AIC`
    - See :py:meth:`~openturns.FittingTest.AICC`
    - See :py:meth:`~openturns.FittingTest.BestModelAIC`
    - See :py:meth:`~openturns.FittingTest.BestModelAICC`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/statistical_tests/plot_fitted_distribution_ranking`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
    - [nisthandbook]_
    - [dagostino1986]_
    - [sprent2001]_
    - [bhattacharyya1997]_
    - [burnham2002]_
