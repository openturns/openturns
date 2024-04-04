.. _regression_analysis:

Regression analysis
-------------------

In this page, we analyse the result from linear regression.
These statistics make it possible to analyse a :ref:`Linear regression<linear_regression>`.

The linear model is:

.. math::

    Y = \sum_{k = 1}^{p} a_k \Psi_k(\vect{x}) + \epsilon

where:

- :math:`n_x \in \Nset` is the dimension of the input vector,
- :math:`\vect{x} \in \Rset^{n_x}` is the input vector,
- :math:`p \in \Nset` is the number of parameters,
- :math:`(\Psi_k)_{k = 1, ..., p}` are the
  basis functions where :math:`\Psi_k: \Rset^{n_x} \rightarrow \Rset` for
  :math:`k \in \{1, ..., p\}`,
- :math:`(a_k)_{k = 1, ..., p}` are the coefficients where
  :math:`a_k \in \Rset` for :math:`k \in \{1, ..., p\}`,
- :math:`\epsilon \sim \cN(0, \sigma^2)` where
  :math:`\cN` is a normal distribution and
  :math:`\sigma > 0` is its standard deviation.

The main goal of considering a normal noise is to be able to make tests of
significance (see [rawlings2001]_ page 3).
In particular, this enables to use the F-test and T-test that
we are going to review later in this document.
Furthermore, if the errors are normal, then the method of least squares
and the maximum likelihood method are equivalent (see [bingham2010]_ theorem
1.8 page 22).

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

Let :math:`n \in \Nset` be the sample size.
A set of observations of the input variables is required:

.. math::

    \cX = \left\{ \vect{x}^{(1)}, \dots, \vect{x}^{(n)} \right\}.

We assume that the random errors :math:`\left\{ \epsilon^{(1)}, \dots, \epsilon^{(n)} \right\}`
are independent.
We consider the corresponding model evaluations:

.. math::

    \cY = \left\{ y^{(1)}, \dots, y^{(n)} \right\}

where:

.. math::

    y^{(i)} = \sum_{k = 1}^{p} a_k \Psi_k \left(\vect{x}^{(i)}\right) + \epsilon^{(i)}


for :math:`i \in \{1, ..., n\}`.
Since the errors :math:`(\epsilon^{(i)})_{i = 1, ..., n}` are independent,
then the output observations :math:`\cY` are independent too.
Let :math:`\vect{y} = (y^{(1)},\dots,y^{(n)})^T \in \Rset^{n}` be the
vector of output observations.

Solution of the least squares problem
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The *design matrix* :math:`\mat{\Psi} \in \Rset^{n \times p}` is the value
of the basis functions over the inputs variables in the sample:

.. math::

    \mat{\Psi}_{ij} = \Psi_j \left(\vect{x}^{(i)}\right)

for :math:`i = 1, ..., n` and :math:`j = 1, ..., p`.
Assume that the design matrix has full rank.
The solution of the linear least squares problem is:

.. math::

    \widehat{\vect{a}}
    = \left(\Tr{\mat{\Psi}} \mat{\Psi}\right)^{-1} \Tr{\mat{\Psi}} \vect{y}.

Statistics
~~~~~~~~~~

Let :math:`\bar{y}` be the sample mean:

.. math::

    \bar{y} = \frac{1}{n} \sum_{j = 1}^n y_j.

The total sum of squares (see [baron2014]_ page 398) is:

.. math::

    SS_{TOT}
    = \sum_{j = 1}^n \left(y_j - \bar{y}_j\right)^2
    = \left(\vect{y} - \bar{\vect{y}}\right)^T \left(\vect{y} - \bar{\vect{y}}\right).

where :math:`\bar{\vect{y}} = (\bar{y}, ..., \bar{y})^T \in \Rset^n`.
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

The coefficient of determination is (see [baron2014]_ page 399):

.. math::

    R^2 = \frac{SS_{REG}}{SS_{TOT}}.

The coefficient of determination measures the part
of the variance explained by the linear regression model.

Variance
~~~~~~~~

The unbiased estimator of the variance is (see [baron2014]_ page 400,
[bingham2010]_ page 67):

.. math::

    \hat{\sigma}^2 = \frac{SS_{ERR}}{n - p}.

ANOVA F-test
~~~~~~~~~~~~

The F-statistic is based on the hypothesis that all coefficients
are simultaneously zero (see [baron2014]_ page 400).
The ANOVA F-test is based on the hypothesis:

.. math::

    H_0 : a_1 = . . . = a_p = 0
    \qquad \textrm{vs} \qquad
    H_A : \textrm{at least one } a_k \neq 0.

Let :math:`\vect{y} \in \Rset^n` be the vector of
observations and :math:`\vect{\hat{y}} \in \Rset^n`
be the vector of predictions.

The F-statistic is (see [bingham2010]_ Kolodziejcyzk’s theorem 6.5 page 154,
[baron2014]_ page 400):

.. math::

    f = \frac{SS_{reg} / p}{SS_{ERR} / (n - p)}.

The p-value is computed from the Fisher-Snedecor
distribution :math:`F_{p, n - p - 1}` (see [baron2014]_ page 400, [faraway2014]_ page 35).

T-test for individual coefficients
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`\hat{\sigma}^2` be the estimator of the variance (see [baron2014]_ page 400):

.. math::

    \hat{\sigma}^2 = \frac{SS_{ERR}}{n - p}.

The variance of the estimator of the parameters is:

.. math::

    \Var{\hat{a}_k} = \hat{\sigma}^2 (\Tr{\mat{\Psi}} \mat{\Psi})^{-1}

Let :math:`\operatorname{SD}(\hat{a}_k)` be the standard deviation of the
estimator of :math:`a_k`:

.. math::

    \operatorname{SD}(\hat{a}_k) = \sqrt{\Var{\hat{a}_k}}

for any :math:`k \in \{1, ..., p\}`.
For :math:`k = 1, ..., p`, the T-test is (see [baron2014]_ page 401):

.. math::

    H_0 : a_k = 0 \qquad \textrm{vs} \qquad H_A : a_k \neq 0.

The T-statistic is (see [baron2014]_ page 401, [rawlings2001]_ eq. 4.47 page 122):

.. math::

    t = \frac{\hat{a}_k}{\operatorname{SD}(\hat{a}_k)}

for :math:`k = 1, ..., p`.
The p-value is computed from the Student's T distribution
with :math:`n - p - 1` degrees of freedom (see [baron2014]_ page 401).


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

