.. _bic:

Bayesian Information Criterion (BIC)
------------------------------------

This method deals with the modelling of a probability distribution of a
random vector :math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`. It
seeks to rank variable candidate distributions by using a sample of data
:math:`\left\{ \vect{x}_1,\vect{x}_2,\ldots,\vect{x}_N \right\}`.
The Bayesian Information Criterion (BIC) allows to
answer this question in the one dimensional case :math:`n_X =1`.

Let us limit the case to :math:`n_X = 1`. Thus we denote
:math:`\vect{X} = X^1 = X`. Moreover, let us denote by :math:`\cM_1`,…,
:math:`\cM_K` the parametric models envisaged by user among the
:ref:`parametric models <parametric_distibution_factories>`. We
suppose here that the parameters of these models have been estimated
previously by :ref:`Maximum Likelihood <maximum_likelihood>`
the on the basis of the sample
:math:`\left\{ \vect{x}_1,\vect{x}_2,\ldots,\vect{x}_n \right\}`. We
denote by :math:`L_i` the maximized likelihood for the model
:math:`\cM_i`.

By definition of the likelihood, the higher :math:`L_i`, the better the
model describes the sample. However, using the likelihood as a criterion
to rank the candidate probability distributions would involve a risk:
one would almost always favour complex models involving many parameters.
If such models provide indeed a large numbers of degrees-of-freedom that
can be used to fit the sample, one has to keep in mind that complex
models may be less robust that simpler models with less parameters.
Actually, the limited available information (:math:`N` data points) does
not allow to estimate robustly too many parameters.

The BIC criterion can be used to avoid this problem. The principle is to
rank :math:`\cM_1,\dots,cM_K` according to the following quantity:

.. math::

   \begin{aligned}
       \textrm{BIC}_i = \log \left( L_i \right) - \frac{p_i}{2} \log(n)
     \end{aligned}

where :math:`p_i` denotes the number of parameters being adjusted for
the model :math:`\cM_i`. The larger :math:`\textrm{BIC}_i`, the better
the model. Note that the idea is to introduce a penalization term that
increases with the numbers of parameters to be estimated. A complex
model will then have a good score only if the gain in terms of
likelihood is high enough to justify the number of parameters used.

The term “Bayesian Information Criterion” comes the interpretation of
the quantity :math:`\textrm{BIC}_i`. In a bayesian context, the unknow
“true” model may be seen as a random variable. Suppose now that the user
does not have any informative prior information on which model is more
relevant among :math:`\cM_1`,…, :math:`\cM_K`; all the models are thus
equally likely from the point of view of the user. Then, one can show
that :math:`\textrm{BIC}_i` is an approximation of the posterior
distribution’s logarithm for the model :math:`\cM_i`.


.. topic:: API:

    - See :class:`~openturns.FittingTest_BIC`
    - See :class:`~openturns.FittingTest_BestModelBIC`

.. topic:: Examples:

    - See :ref:`examples/data_analysis/fitted_distribution_ranking.ipynb`

.. topic:: References:

    - [Saporta1990]_
    - [Dixon1983]_
    - [NISThandbook]_
    - [Dagostino1986]_
    - [Sprent2001]_
    - [Bhattacharyya1997]_
    - [Burnham2002]_
