.. _bic:

Bayesian Information Criterion (BIC)
------------------------------------

This method can be used to rank candidate distributions with respect to data
:math:`\left\{ \inputReal_1,\inputReal_2,\ldots,\inputReal_{\sampleSize} \right\}`.

We denote by :math:`\cM_1, \dots, \cM_K` the :math:`K` parametric models we want to test
(see :ref:`parametric models <parametric_distribution_factories>`).
We suppose here that the parameters of these models have been estimated from the sample (see
:ref:`Maximum Likelihood <maximum_likelihood>`). We
denote by :math:`L_i` the maximized likelihood of the sample with respect to
the model :math:`\cM_i`.

By definition of the likelihood, the higher :math:`L_i`, the better the
model describes the sample. However, relying entirely on the value of the likelihood
runs the risk of systematically selecting the model with the most parameters. As a matter of fact,
the greater the number of parameters, the easier it is for the distribution to adapt to the data.

The Bayesian Information Criterion (BIC) can be used to avoid this problem. It is also referred to
in the literature as the Schwarz information criterion. It is an information criterion derived from the
Akaike information criterion (see :ref:`aic`). Note that the library divides the BIC
defined in the literature by the sample size, which has no impact on the selection of the best model.

The BIC of the model :math:`\cM_i` is defined in the library by:

.. math::

   \begin{aligned}
       \textrm{BIC}(\cM_i) = -2 \frac{\log(L_i)}{\sampleSize} + \frac{k_i \log(\sampleSize)}{\sampleSize}
     \end{aligned}

where :math:`k_i` denotes the number of parameters of the model :math:`\cM_i`
that have been inferred from the sample. The smaller :math:`\textrm{BIC}(\cM_i)`, the better
the model:

.. math::

   \cM_{BIC} = \argmin_{\cM_i, 1\leq i \leq K} {BIC}(\cM_i)

The idea is to introduce a penalization term that
increases with the numbers of parameters to be estimated. A complex
model will then have a good score only if the gain in terms of
likelihood is high enough to justify the number of parameters used.

The term "Bayesian Information Criterion" comes from the interpretation of
the quantity :math:`\textrm{BIC}(\cM_i)`. In a bayesian context, the unknown
"true" model may be seen as a random variable. Suppose now that the user
does not have any informative prior information on which model is more
relevant among :math:`\cM_1, \dots, \cM_K`; all the models are thus
equally likely from the point of view of the user. Then, one can show
that :math:`\textrm{BIC}(\cM_i)` is an approximation of the posterior
distribution's logarithm for the model :math:`\cM_i`.

This criterion is a valuable criterion to reject
a model which is not relevant, but can be tricky to interpret in
some cases.
For example, if two models have very close BIC, these two models should
be considered instead of keeping only the model which has the lowest BIC.


.. topic:: API:

    - See :py:func:`~openturns.FittingTest.BIC`
    - See :py:func:`~openturns.FittingTest.BestModelBIC`

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
