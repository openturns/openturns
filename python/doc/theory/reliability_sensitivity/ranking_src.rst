.. _ranking_src:

Uncertainty ranking: SRC and SRRC
---------------------------------

Standard Regression Coefficients (SRC) deal with analyzing the influence the random vector
:math:`\vect{X} = \left( X_1,\ldots,X_{n_X} \right)` has on a random
variable :math:`Y`.
We measure linear relationships that exist between :math:`Y`
and the different input variables :math:`X_i`.

The principle of the multiple linear regression model consists in
attempting to find the function that links the
variable :math:`Y` to the :math:`n_x` variables
:math:`X_1,\ldots,X_{n_X}` by means of a linear model:

.. math::

    Y = a_0 + \sum_{i=1}^{n_X} a_i X_i + \varepsilon,

where :math:`(a_i)_{i = 0, 1, ..., n_X}` are constant parameters,
:math:`\varepsilon` is a random variable with zero mean
and standard deviation :math:`\sigma_{\varepsilon}` independent of the
input variables :math:`X_i`. If the random variables
:math:`X_1,\ldots,X_{n_X}` are independent and with finite variance
:math:`\Var{X_i} = \sigma_i^2`, the variance of :math:`Y` can be
estimated as follows:

.. math::

    \Var{Y} = \sum_{i=1}^n a_i^2 \Var{X_i} + \sigma_{\varepsilon}^2 = \sigma^2.

The SRC coefficient is defined by (see [borgonovo2017]_ page 131, eq. 14.3):

.. math::

    \operatorname{SRC}_i = a_i \sqrt{\frac{\Var{X_i}}{\Var{Y}}}

for :math:`i = 1, ..., n_X`.
The estimators for the regression coefficients
:math:`a_0,\ldots,a_{n_X}`, and the standard deviation
:math:`\sigma` are obtained from a sample of
:math:`(Y,X_1,\ldots,X_{n_X})`.
The SRC coefficients are defined as the estimators :math:`\widehat{C}_i`
of the coefficients :math:`SRC_i`:

.. math::

    \widehat{\operatorname{SRC}}_i = \widehat{a}_i \frac{\widehat{\sigma}_i}{\widehat{\sigma}},

for :math:`i = 1, ..., n_X`,
where :math:`\widehat{a}_i` is the estimate of the regression coefficient :math:`a_i`,
:math:`\widehat{\sigma}_i` is the sample standard
deviation of the sample of the input variable :math:`X_i`
and :math:`\widehat{\sigma}` is the sample standard
deviation of the sample of the output variable :math:`Y`.
The absolute value of this estimated
contribution is by definition between 0 and 1. The closer it is to 1,
the greater the impact the variable :math:`X_i` has on the variance of
:math:`Y`.
See the :class:`~openturns.CorrelationAnalysis.computeSRC` method to compute the SRC
coefficients.

Before estimating the SRC coefficients,
we mush check the quality of the :ref:`linear regression <linear_regression>`:
if the linear regression model
is a poor fit to the data, then the SRC coefficients are useless.
See e.g. the :class:`~openturns.MetaModelValidation` class to validate
the linear model against a test data set.

The :math:`\operatorname{SRC}_i^2` index, which is the contribution of :math:`X_i`
to the variance of :math:`Y`, is sometimes described in
the literature as the “importance factor”, because of the similarity
between this approach to linear regression and the method of cumulative
variance which uses the term importance factor.
This importance factor is also named "squared SRC" coefficient
(see [borgonovo2017]_ page 131, eq. 14.5):

.. math::

    \operatorname{SRC}_i^2 = a_i^2 \frac{\Var{X_i}}{\Var{Y}}

for :math:`i = 1, ..., n_X`.
The squared SRC coefficients of a linear model are equal to its
:ref:`Sobol' indices <sensitivity_sobol>`.
If the model is linear, the first-order Sobol' index is equal
to the total Sobol' index since there is no interaction.
See the :class:`~openturns.CorrelationAnalysis.computeSquaredSRC` method to compute the squared SRC
coefficients.

If the input random variables :math:`(X_i)_{i = 1, ..., n_X}` are dependent,
then the SRC is not a valid importance measure anymore (see [daveiga2022]_ remark 4
page 33).
In this case, the partial correlation coefficient (PCC) has been suggested, but
this index is more a measure of the linear relationship between the input and the
output.
Other indices such as the Lindeman-Merenda-Gold (LMG) have been suggested in the
dependent case (see [daveiga2022]_ page 33).

Standard *Rank* Regression Coefficients (SRRC) are SRC coefficients
computed on the ranked input variables
:math:`\operatorname{rank}(\vect{X}) = \left( \operatorname{rank}(X_1), \ldots, \operatorname{rank}(X_{n_X}) \right)`
and the ranked output variable :math:`\operatorname{rank}(Y)`.
They are useful when the relationship between :math:`Y`
and :math:`\vect{X}` is not linear (so SRC cannot be used),
but only monotonic.
See the :class:`~openturns.CorrelationAnalysis.computeSRRC` method to compute the SRRC
coefficients.

.. topic:: API:

    - See :meth:`~openturns.CorrelationAnalysis.computeSRC`
    - See :meth:`~openturns.CorrelationAnalysis.computeSRRC`
    - See :meth:`~openturns.CorrelationAnalysis.computeSquaredSRC`


.. topic:: Examples:

    - See :doc:`/auto_data_analysis/manage_data_and_samples/plot_sample_correlation`


.. topic:: References:

    - [saltelli2000]_
    - [helton2003]_
    - [kleijnen1999]_
    - [borgonovo2017]_
    - [daveiga2022]_
