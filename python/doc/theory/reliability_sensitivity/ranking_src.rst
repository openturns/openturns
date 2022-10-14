.. _ranking_src:

Uncertainty ranking: SRC and SRRC
---------------------------------

Standard Regression Coefficients (SRC) deal with analyzing the influence the random vector
:math:`\vect{X} = \left( X_1,\ldots,X_{n_X} \right)` has on a random
variable :math:`Y` which is being studied for uncertainty. Here we
attempt to measure linear relationships that exist between :math:`Y`
and the different components :math:`X_i`.

The principle of the multiple linear regression model consists in
attempting to find the function that links the
variable :math:`Y` to the :math:`n_x` variables
:math:`X_1,\ldots,X_{n_X}` by means of a linear model:

.. math::

    Y = a_0 + \sum_{i=1}^{n_X} a_i X_i + \varepsilon,

where :math:`\varepsilon` describes a random variable with zero mean
and standard deviation :math:`\sigma_{\varepsilon}` independent of the
input variables :math:`X_i`. If the random variables
:math:`X_1,\ldots,X_{n_X}` are independent and with finite variance
:math:`\Var{X_k} = (\sigma_i)^2`, the variance of :math:`Y` can be
estimated as follows:

.. math::

    \Var{Y} = \sum_{i=1}^n (a_i)^2 \Var{X_i} + (\sigma_{\varepsilon})^2 = (\sigma)^2.

From this we obtain the following coefficients:

.. math::

    C_i = a_i \sqrt{\frac{\Var{X_k}}{\Var{Y}}}

The estimators for the regression coefficients
:math:`a_0,\ldots,a_{n_X}`, and the standard deviation
:math:`\sigma` are obtained from a sample of
:math:`(Y,X_1,\ldots,X_{n_X})`.
The SRC coefficients are defined as the estimators :math:`\widehat{C}_i`
of the coefficients :math:`C_i`:

.. math::

    \widehat{C}_i = \frac{\displaystyle \widehat{a}_i \widehat{\sigma}_i}{\displaystyle \widehat{\sigma}},


where :math:`\widehat{a}_i` denotes the estimate of the regression coefficient :math:`a_i`,
:math:`\widehat{\sigma}_i` denotes the empirical standard
deviation of the sample of the input variable :math:`X_i`
and :math:`\widehat{\sigma}` denotes the empirical standard
deviation of the sample of the output variable :math:`Y`.
The absolute value of this estimated
contribution is by definition between 0 and 1. The closer it is to 1,
the greater the impact the variable :math:`X_i` has on the dispersion of
:math:`Y`.

The square :math:`(C_i)^2`, which is the contribution of :math:`X_i`
to the variance of :math:`Y`,
is sometimes described in
the literature as the “importance factor”, because of the similarity
between this approach to linear regression and the method of cumulative
variance which uses the term importance factor.

It is a good idea to check the quality of the :ref:`linear regression <linear_regression>`
before estimating the SRC coefficients: if the linear regression model
is a poor fit to the data, then the SRC coefficients are useless.

Note that if there exists a map :math:`g` such that :math:`Y=g(X_1, ..., X_{n_X})`,
then the squared SRC coefficients are equal to :ref:`Sobol' indices <sensitivity_sobol>`.

Standard *Rank* Regression Coefficients (SRRC) are SRC coefficients
computed on the ranked input variables
:math:`r\vect{X} = \left( rX_1,\ldots,rX_{n_X} \right)`
and the ranked output variable :math:`rY`.
They are useful when the relationship between :math:`Y`
and :math:`\vect{X}` is not linear (so SRC cannot be used),
but only monotonic.

.. topic:: API:

    - See :meth:`~openturns.CorrelationAnalysis.computeSRC`
    - See :meth:`~openturns.CorrelationAnalysis.computeSRRC`


.. topic:: Examples:

    - See :doc:`/auto_data_analysis/manage_data_and_samples/plot_sample_correlation`


.. topic:: References:

    - [saltelli2000]_
    - [helton2003]_
    - [kleijnen1999]_
