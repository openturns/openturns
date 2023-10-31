.. _regression_analysis:

Regression analysis
-------------------

In this page, we analyse the result from linear regression.
These statistics make it possible to analyse a :ref:`polynomial_least_squares`.

The linear model is:

.. math::

    y  \sim \sum_{k = 1}^{p} a_k \psi_k(\vect{x}) + \epsilon

where:

- :math:`n_x \in \Nset` is the dimension of the input vector,
- :math:`\vect{x} \in \Rset^{n_x}` is the input vector,
- :math:`p \in \Nset` is the number of parameters,
- :math:`\{\psi_k : \Rset^{n_x} \rightarrow \Rset\}_{k = 1, ..., p}` are the
  basis functions,
- :math:`\{a_k \in \Rset\}_{k = 1, ..., p}` are the coefficients,
- :math:`\epsilon \sim \cN(0, \sigma^2)` where
  :math:`\cN` is the Gaussian distribution and
  :math:`\sigma > 0` is the standard deviation.

Caution
~~~~~~~

There is an ambiguity when the number of parameters is unspecified in the
text, which may explain the differences between the various formulas
we find in the books.
In some texts, the intercept has the index 0, which leads to an
increased number of parameters.
In the present document, the number of parameters is equal to :math:`p`,
but not all books use the same convention:

- in [baron2014]_ (eq. 11.11 page 396), in [sen1990]_ (eq. 2.1 page 28)
  and in [rawlings2001]_ (eq. 3.1 page 75),
  the number of parameters is equal to :math:`p + 1`,
- in [bingham2010]_ (page 61) and in [faraway2014]_ (page 15),
  the number of parameters is equal to :math:`p`.

Experimental design
~~~~~~~~~~~~~~~~~~~

A set of observations of input parameters, is required:

.. math::

    \cX = \left\{ \vect{x}^{(1)}, \dots, \vect{x}^{(N)} \right\},

as well as the corresponding model evaluations:

.. math::

    \cY = \left\{ y^{(1)},\dots,y^{(N)} \right\}.

Let :math:`\vect{y} = (y^{(1)},\dots,y^{(N)})^T \in \Rset^{N}` be the
vector of output observations.

Statistics
~~~~~~~~~~

Let :math:`\bar{y}` be the sample mean:

.. math::

    \bar{y} = \frac{1}{n} \sum_{j = 1}^n y_j.

The total sum of squares ([baron2014]_ page 398) is:

.. math::

    SS_{TOT}
    = \sum_{j = 1}^n \left(y_j - \bar{y}_j\right)^2
    = \left(\vect{y} - \bar{\vect{y}}\right)^T \left(\vect{y} - \bar{\vect{y}}\right).

where :math:`\hat{\vect{y}} = (\bar{y}, ..., \bar{y})^T \in \Rset^n`.

The regression sum of squares is:

.. math::

    SS_{REG}
    = \sum_{j = 1}^n \left(\hat{y}_j - \bar{y}_j\right)^2
    = \left(\hat{\vect{y}} - \bar{\vect{y}}\right)^T \left(\hat{\vect{y}} - \bar{\vect{y}}\right).

The error sum of squares is:

.. math::

    SS_{ERR}
    = \sum_{j = 1}^n \left(y_j - \hat{y}_j\right)^2
    = \left(\vect{y} - \hat{\vect{y}}\right)^T \left(\vect{y} - \hat{\vect{y}}\right).

Coefficient of determination
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The coefficient of determination is ([baron2014]_ page 399):

.. math::

    R^2 = \frac{SS_{REG}}{SS_{TOT}}.

The coefficient of determination measures the part
of the variance explained by the linear regression model.

Variance
~~~~~~~~

The unbiased estimator of the variance is ([baron2014]_ page 400,
[bingham2010]_ page 67):

.. math::

    \hat{\sigma}^2 = \frac{SS_{ERR}}{n - p}.

ANOVA F-test
~~~~~~~~~~~~

The F-statistic is based on the hypothesis that all coefficients
are simultaneously zero ([baron2014]_ page 400).
The ANOVA F-test is based on the hypothesis:

.. math::

    H_0 : a_1 = . . . = a_p = 0
    \qquad \textrm{vs} \qquad
    H_A : \textrm{at least one } a_k \neq 0.

Let :math:`n \in \Nset` be the sample size.
Let :math:`\vect{y} \in \Rset^n` be the vector of
observations and :math:`\vect{\hat{y}} \in \Rset^n`
be the vector of predictions.

The F-statistic is ([bingham2010]_ Kolodziejcyzk’s theorem 6.5 page 154,
[baron2014]_ page 400):

.. math::

    F = \frac{SS_{reg} / p}{SS_{ERR} / (n - p)}.

The p-value is computed from the Fisher-Snedecor
distribution :math:`F_{p, n - p - 1}` ([baron2014]_ page 400, [faraway2014]_ page 35).

T-test for individual coefficients
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`\hat{\sigma}^2` be the estimator of the variance ([baron2014]_ page 400):

.. math::

    \hat{\sigma}^2 = \frac{SS_{ERR}}{n - p}.

Let :math:`\hat{\sigma}^2(a_k)` be the diagonal of the
covariance matrix :math:`\sigma^2 (\Psi^T \Psi)^{-1}`:

.. math::

    \hat{\sigma}^2(a_k) = \hat{\sigma}^2 \left(\Psi^T \Psi\right)^{-1}_{kk}

for :math:`k = 1, ..., p`.
For :math:`k = 1, ..., p`, the test is ([baron2014]_ page 401):

.. math::

    H_0 : a_k = 0 \qquad \textrm{vs} \qquad H_A : a_k \neq 0.

The T-statistic is ([baron2014]_ page 401, [rawlings2001]_ eq. 4.47 page 122):

.. math::

    t = \frac{\hat{a}_k}{\hat{\sigma}(a_k)}

for :math:`k = 1, ..., p`.
The p-value is computed from the Student's T distribution
with :math:`n - p - 1` degrees of freedom ([baron2014]_ page 401).


.. topic:: API:

    - See :class:`~openturns.LinearModelAnalysis`

.. topic:: Examples:

    - See :doc:`/auto_meta_modeling/general_purpose_metamodels//plot_linear_model`


.. topic:: References:

    - [baron2014]_
    - [faraway2014]_
    - [rawlings2001]_
    - [sen1990]_
    - [bingham2010]_

