.. _aic:

Akaike Information Criterion (AIC)
----------------------------------

This method deals with the modelling of a probability distribution of a
random variable :math:`X`. It seeks to rank variable candidate 
distributions by using a sample of data
:math:`\left\{ \vect{x}_1,\vect{x}_2,\ldots,\vect{x}_N \right\}`.

We denote by :math:`\cM_1`,…, :math:`\cM_K` the parametric models 
envisaged by user among the :ref:`parametric models <parametric_distibution_factories>`. 
We suppose here that the parameters of these models have been estimated
previously by :ref:`Maximum Likelihood <maximum_likelihood>`
the on the basis of the sample
:math:`\left\{ \vect{x}_1,\vect{x}_2,\ldots,\vect{x}_n \right\}`. We
denote by :math:`L_i` the maximized likelihood for the model
:math:`\cM_i`.

By definition of the likelihood, the higher :math:`L_i`, the better the
model describes the sample. However, using the likelihood as a criterion
to rank the candidate probability distributions would involve a risk:
one would almost always favor complex models involving many parameters.
If such models provide indeed a large numbers of degrees-of-freedom that
can be used to fit the sample, one has to keep in mind that complex
models may be less robust that simpler models with less parameters.
Actually, the limited available information (:math:`N` data points) does
not allow to estimate robustly too many parameters.

The Akaike Information Criterion (AIC) can be used to avoid this problem. 
The principle is to rank :math:`\cM_1,\dots,\cM_K` according to the following quantity:

.. math::

   \begin{aligned}
       \textrm{AIC}_i = -2 \frac{\log(L_i)}{n} + \frac{2 p_i}{n} 
     \end{aligned}

where :math:`p_i` denotes the number of parameters being adjusted for
the model :math:`\cM_i`. The smaller :math:`\textrm{AIC}_i`, the better
the model. Note that the idea is to introduce a penalization term that
increases with the numbers of parameters to be estimated. A complex
model will then have a good score only if the gain in terms of
likelihood is high enough to justify the number of parameters used.

In context of small data, there is a substantial risk that AIC select 
models that have too many parameters. In other words, the risk of 
overfitting is important. To tackle such issue, the AICc criterion was 
developed : it consists in evaluating the AIC with a correction term (
extra penalty) for small data. The formula is as follows :

.. math::

   \begin{aligned}
       \textrm{AICC}_i = AIC + \frac{(2 p_i)(p_i + 1)}{n - p_i - 1}
     \end{aligned}

One might notice that the extra term penalty vanishes for 
:math:`n \rightarrow \infty`.


.. topic:: API:

    - See :class:`~openturns.FittingTest_AIC`
    - See :class:`~openturns.FittingTest_AICC`
    - See :class:`~openturns.FittingTest_BestModelAIC`
    - See :class:`~openturns.FittingTest_BestModelAICC`

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
