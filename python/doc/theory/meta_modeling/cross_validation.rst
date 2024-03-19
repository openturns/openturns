.. _cross_validation:

Validation and cross validation of surrogate models
---------------------------------------------------

Introduction
~~~~~~~~~~~~

In ordinary validation of a surrogate model, we hold some observations from the
sample and train the model on the remaining observations.
Then we test the surrogate model against the hold out observations.
One of the problems of this method is that the surrogate
model is trained using only a subset of the available data.
As a consequence, the estimated error can be pessimistic.
In cross-validation, the roles of the train and test samples are exchanged.
Hence, more of the available data is used, resulting in an improved
accuracy of the error of the metamodel.
One of the problems of cross-validation is that we have to train
the surrogate model several times which can be time consuming.
In this case, the fast methods that we present below can be useful.

Let :math:`\physicalmodel: \set{X} \rightarrow \Rset` be a physical
model where :math:`\set{X} \subseteq \Rset^{n_x}`
is the domain of the input parameters :math:`\vect{x}`.
Let :math:`\metamodel` be a surrogate model of :math:`\physicalmodel`, i.e.
an approximation of the function.
Once a polynomial response surface :math:`\metamodel`
of the original numerical model :math:`\physicalmodel` has been
built up, we may estimate the mean squared error, i.e. the
discrepancy between the response surface and the true model response
in terms of the weighted :math:`L_2`-norm:

.. math::
   \operatorname{MSE} \left(\metamodel\right)
   & = \left\| \physicalmodel(\vect{x}) - \metamodel(\vect{x}) \right\|_{L_2}^2(\set{X}) \\
   & = \int_{\set{X}} \left[ \physicalmodel(\vect{x}) - \metamodel(\vect{x}) \right]^2  \pdf d\vect{x}

where :math:`\probabilitydensity` is the probability density function
of the random vector :math:`\vect{X}`.
In this section, we present the cross-validation of linear least squares
model, as presented in [wang2012]_ page 485.

The fraction of variance unexplained is:

.. math::

    \operatorname{FVU}
    = \frac{\operatorname{MSE}\left(\metamodel\right)}{\Var{Y}}

where :math:`\Var{Y}` is the variance of the random output :math:`Y = \physicalmodel(\vect{X})`.
The fraction of variance unexplained is a relative mean squared error.
The coefficient of determination is:

.. math::

    R^2 & = 1 - \operatorname{FVU} \\
    & = 1 - \frac{\operatorname{MSE}\left(\metamodel\right)}{\Var{Y}}

Naive cross-validation
~~~~~~~~~~~~~~~~~~~~~~

In the ordinary or naive validation method, we divide the data sample (i.e.
the experimental design) into two independent sub-samples:
the training and the test set.
In this case, the metamodel :math:`\metamodel`
is built from one sub-sample, i.e. the *training set*, and its
performance is assessed by comparing its predictions to the other
subset, i.e. the *test set*.
A single split will lead to a validation estimate.
When several splits are created, the cross-validation error
estimate is obtained by averaging over the splits.
When the coefficient of determination is evaluated on a test set which is
independent from the training set, the coefficient of determination is
called the :math:`Q^2` score.

.. figure:: ../../_static/train_test_cross_validation.png
    :align: center
    :alt: Validation of a surrogate
    :width: 50%

    *Figure 1.* Validation using one single split.


Let :math:`n \in \Nset` be an integer representing the sample size.
Let :math:`\set{D} \subseteq \set{X}` be a set of :math:`n` independent
observations of the random vector :math:`\vect{X}`:

.. math::
    \set{D} = \left\{\vect{x}^{(1)}, ..., \vect{x}^{(n)} \in \Rset^{n_x}\right\}

and consider the corresponding outputs of the model:

.. math::
    \left\{y_1, ..., y_n  \in \Rset\right\}

where:

.. math::
    y^{(j)} = \physicalmodel\left(\vect{x}^{(j)}\right)

for :math:`j = 1, ..., n`.
The Monte-Carlo estimator of the mean squared error is:

.. math::
    \widehat{\operatorname{MSE}}\left(\metamodel\right)
    = \frac{1}{n} \sum_{j = 1}^n \left[ \physicalmodel\left(\vect{x}^{(j)}\right) - \metamodel \left(\vect{x}^{(j)}\right) \right]^2

If the test set :math:`\set{D}` is not independent from the training set
(the set used to calibrate the metamodel), then the previous estimator
may underestimate the true value of the mean squared error.
In order to create a test set independent from the training set, a
simple method is to split the data set into two parts.
The drawback of this method is that this reduces the size of the training
set, so that the mean squared error evaluated on the test set can be pessimistic.
The leave-one-out and K-Fold cross validation methods
use all the data available.

The sample relative mean squared error is:

  .. math::

      \widehat{\varepsilon}_{MSE}
      = \frac{\widehat{\operatorname{MSE}}}{\widehat{\sigma}^2(Y)}

where :math:`\widehat{\sigma}^2(Y)` is the sample variance of the random output:

.. math::

      \widehat{\sigma}^2(Y)
      = \frac{1}{n - 1} \sum_{j = 1}^n \left( y^{(j)} -   \bar{y} \right)^2

where :math:`\bar{y}` is the sample mean of the output:

.. math::

    \bar{y} = \frac{1}{n} \sum_{j = 1}^n y^{(j)}.

Naive and fast cross-validation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The simplest method splits the data into a training set and a test set.
This has several drawbacks. Two other estimators are the leave-one-out and
K-Fold estimators. When implemented naively, this may required to build many
surrogate models. Fortunately, there are *shortcuts* for many surrogate models
including linear least squares regression, splines (and others).
For a linear least squares regression model, we can use the
Sherman-Morrisson-Woodbury formula to get updates of the inverse Gram matrix.
In this document, we present the fast leave-one-out and K-Fold estimators.

Leave-one-out error
~~~~~~~~~~~~~~~~~~~

In this section, we present the naive leave-one-out error estimator,
also known as jackknife in statistics.
Let :math:`\metamodel^{(-j)}` be the surrogate model estimated from the
leave-one-out experimental design :math:`\set{D} \setminus \{\vect{x}^{(j)}\}`.
This is the experimental design where the :math:`j`-th observation
:math:`\vect{x}^{(j)}` is set aside.
The corresponding set of observation indices is:

.. math::
    \set{S}_j = \{1, ..., j - 1, j + 1, ..., n\},

the corresponding input observations are:

.. math::
    \set{D} \setminus \{\vect{x}^{(j)}\}
    = \left\{\vect{x}^{(1)}, ..., \vect{x}^{(j - 1)},
    \vect{x}^{(j + 1)}, ..., \vect{x}^{(n)}\right\}.

and the corresponding output observations are:

.. math::
    \left\{y^{(1)}, ..., y^{(j - 1)}, y^{(j + 1)}, ..., y^{(n)}\right\}.

The leave-one-out residual is defined as the difference between the model evaluation at
:math:`\vect{x}^{(j)}` and its leave-one-out prediction :math:`\metamodel^{(-j)}`:

.. math::

    \Delta^{(j)}
    = \physicalmodel\left(\vect{x}^{(j)}\right) - \metamodel^{(-j)}\left(\vect{x}^{(j)}\right)

We repeat this process for all observations in the experimental
design and obtain the predicted residuals
:math:`\Delta^{(j)}` for :math:`j = 1, \dots, n`.
Finally, the LOO mean squared error estimator is:

.. math::

    \widehat{\operatorname{MSE}}_{LOO}
    & =  \frac{1}{n} \sum_{j = 1}^n \left[ \Delta^{(j)} \right]^2 \\
    & =  \frac{1}{n} \sum_{j = 1}^n \left[ \physicalmodel\left(\vect{x}^{(j)}\right) - \metamodel^{(-j)}\left(\vect{x}^{(j)}\right) \right]^2

One of the drawbacks of the naive method is that it may require
to estimate :math:`n` different surrogate models.
If :math:`n` is large or if training each surrogate model is costly,
then the leave-one-out method can be impractical.
If, however, the surrogate model is based on linear least squares
regression, then the leave-one-out error may be computed much more efficiently, as
shown in the next section.

Fast leave-one-out
~~~~~~~~~~~~~~~~~~

In this section, we present the fast leave-one-out error estimator
of a linear regression least squares model.
In the special case of of a linear regression model, [stone1974]_ (eq. 3.13 page 121)
showed that the leave-one-out residuals have an expression which depends on the diagonal
of the projection matrix.
In this case, the evaluation of the leave-one-out mean squared error involves the
multiplication of the raw residuals by a correction which involves the leverages
of the model.
This method makes it possible to evaluation directly the mean squared error without
necessarily estimating the coefficients of $n$ different leave-one-out regression model.
It is then much faster than the naive leave-one-out method.

Let :math:`m \in \Nset` be an integer representing the number of
parameters in the model.
Assume that the physical model is linear:

.. math::

    \physicalmodel(\vect{x}) = \boldsymbol{D} \vect{a}

for any :math:`\vect{x} \in \set{X}`
where :math:`\vect{a} \in \Rset^m` is the vector of parameters
and :math:`\boldsymbol{D} \in \Rset^{n \times m}` is the
design matrix.
For a linear model, the columns of the design matrix correspond
to the input parameters and the rows correspond to the observations:

.. math::
    \boldsymbol{D}
    =
    \begin{pmatrix}
    1      & x_1^{(1)} & \ldots  & x_{n_x}^{(1)} \\
    \vdots &           &         & \vdots \\
    1      & x_1^{(n)} & \ldots  & x_{n_x}^{(n)}
    \end{pmatrix}
    \in \Rset^{n \times {(n_x + 1)}}.

In the previous equation, notice that the design matrix depends on the
experimental design :math:`\set{D}`.

Assume that the matrix :math:`\boldsymbol{D}` has full rank.
The solution of the linear least squares problem is
given by the normal equations ([Bjorck1996]_ eq. 1.1.15 page 6):

.. math::

    \widehat{\vect{a}} = \left(\boldsymbol{D}^T \boldsymbol{D} \right)^{-1} \boldsymbol{D}^T \vect{y}.

Consider the linear surrogate model:

.. math::

    \metamodel(\vect{x}) = \boldsymbol{D} \widehat{\vect{a}}

for any :math:`\vect{x} \in \set{X}` where :math:`\widehat{\vect{a}}` is the
estimate from linear least squares.
We substitute the estimator in the previous equation and
get the value of the surrogate linear regression model:

.. math::

    \metamodel(\vect{x})
    = \boldsymbol{D} \left(\boldsymbol{D}^T \boldsymbol{D} \right)^{-1} \boldsymbol{D}^T \vect{y}

Let :math:`\boldsymbol{H}` be the projection ("hat") matrix ([wang2012]_ eq. 16.8 page 472):

.. math::

    \boldsymbol{H}
    = \boldsymbol{D} \left(\boldsymbol{D}^T \boldsymbol{D} \right)^{-1} \boldsymbol{D}^T
    \in \Rset^{n \times n}.


Hence, the value of the linear regression model is the matrix-vector product:

.. math::

    \widehat{\vect{y}} = \metamodel(\vect{x}) = \boldsymbol{H} \vect{y}.

We can prove that the LOO residual is:

.. math::

    y^{(j)} - \widehat{g}^{(-j)}(\vect{x}^{(j)})
    = \frac{y^{(j)} - \widehat{g}(\vect{x}^{(j)})}{1 - h_{jj}}

where :math:`h_{jj}` is the :math:`j`-th diagonal term of the hat matrix,
also known as the *leverage*.
In other words, the residual error of the LOO surrogate model is equal to the
residual of the full surrogate model corrected by :math:`1 - h_{jj}`.
This avoids to actually build the LOO surrogate.
We substitute the previous expression in the definition of the leave-one-out
mean squared error estimator and get the fast leave-one-out cross validation
error ([wang2012]_ eq. 16.25 page 487):

.. math::
    \widehat{\operatorname{MSE}}_{LOO}
    = \frac{1}{n} \sum_ {j = 1}^n \left( \frac{y^{(j)} -
          \widehat{g}(\vect{x}^{(j)})}{1 - h_{jj}} \right)^2

Corrected leave-one-out
~~~~~~~~~~~~~~~~~~~~~~~

A penalized variant of the leave-one-out mean squared error may be used in order to
increase its robustness with respect to overfitting, i.e. to penalize a
large coefficients in the surrogate model compared to the size of the
experimental design.
The corrected leave-one-out error is ([chapelle2002]_, [blatman2009]_ eq. 4.38 page 86):

.. math::
    \widehat{\operatorname{MSE}}_{LOO}^{*}
    = \widehat{\operatorname{MSE}}_{LOO} T(m, n)

where the penalty factor is:

.. math::
    T(m, n)
    = \frac{n}{n - m}  \left(1 + \frac{\operatorname{Tr} \left( \boldsymbol{C}_{emp}^{-1}  \right) }{n} \right)

where :math:`\boldsymbol{C}_{emp}` is the matrix:

.. math::

    \boldsymbol{C}_{emp} = \frac{1}{n}\boldsymbol{\Psi}^{\textsf{T}}\boldsymbol{\Psi}

and :math:`\operatorname{Tr}` is the trace operator.

K-fold cross-validation
~~~~~~~~~~~~~~~~~~~~~~~

In this section, we present the naive K-Fold cross-validation.
Let :math:`k \in \Nset` be a parameter representing the number of
splits in the data set.
The :math:`k`-fold cross-validation technique relies on splitting the
data set :math:`\set{D}` into :math:`k` mutually disjoint sub-samples
:math:`\set{D}_1, \dots, \set{D}_k`, called the *folds*.
The corresponding set of indices:

.. math::

    \set{S}_1 \; \dot{\cup} \; \cdots \; \dot{\cup} \; \set{S}_k
    = \{1, ..., n\}

and the corresponding set of input observations is:

.. math::

    \set{D} = \set{D}_1 \; \dot{\cup} \; \cdots \; \dot{\cup} \; \set{D}_k.

The next figure presents this type of cross validation.

.. figure:: ../../_static/kfold_validation.png
    :align: center
    :alt: K-Fold validation
    :width: 75%

    *Figure 2.* K-Fold cross-validation.

The :math:`k` folds are generally chosen so that they are of
approximately equal size.
If the sample size :math:`n` is a multiple of :math:`k`, then the
folds can have exactly the same size, but this is not necessarily true.

For any :math:`\ell \in \{1, ..., k\}`, let :math:`\metamodel^{(-\set{D}_{\ell})}`
be the surrogate model estimated on the K-fold sample
:math:`\set{D} \setminus \set{D}_{\ell}`.
The local approximation error is estimated on the sample :math:`\set{D}_{\ell}`:

.. math::
   \widehat{\operatorname{MSE}}^{(\ell)}
   = \frac{1}{n_\ell}  \sum_{j \in \set{S}_\ell}
   \left[ \physicalmodel\left(\vect{x}^{(j)}\right) - \metamodel^{(-\set{D}_{\ell})} \left(\vect{x}^{(j)}\right) \right]^2.

where :math:`n_\ell` is the number of observations in
the sub-sample :math:`\set{D}_{\ell}`:

.. math::

    n_{\ell} = \operatorname{card}\left(\set{D}_{\ell} \right).

In the previous equation, the *predicted residual*
:math:`\physicalmodel\left(\vect{x}^{(j)}\right) - \metamodel^{(-\set{D}_{\ell})} \left(\vect{x}^{(j)}\right)`
is the difference between the
evaluation of :math:`\physicalmodel` and the value of the K-Fold surrogate
:math:`\metamodel^{(-\set{D}_{\ell})}` at the point :math:`\vect{x}^{(j)}`.

For any :math:`\ell \in \{ 1, \dots, k\}`, the K-Fold mean square error :math:`\widehat{\operatorname{MSE}}^{(\ell)}` is estimated using
the training set :math:`\set{D} \setminus \set{D}_{\ell}` and
the test set :math:`\set{D}_{\ell}`.
Finally, the global K-fold cross-validation error estimate is the
sample mean ([burman1989]_ page 505):

.. math::
  :label: kfoldMean

   \widehat{\operatorname{MSE}}_{KFold}
   = \sum_{\ell = 1}^{k} \frac{n_{\ell}}{n} \widehat{\operatorname{MSE}}^{(\ell)}

The weight :math:`n_{\ell} / n` reflects the fact that a fold containing
more observations weighs more in the estimator.
The K-Fold error estimate can be obtained
with a single split of the data :math:`\set{D}` into :math:`k` folds.
The *leave-one-out* (LOO) cross-validation is a special case of
K-Fold cross-validation where the number of folds :math:`k` is
equal to :math:`n`, the sample size of the experimental design
:math:`\set{D}`.

We substitute the previous equation in the definition of the K-Fold MSE and get:

.. math::

    \widehat{\operatorname{MSE}}_{KFold}
    & = \sum_{\ell = 1}^k \frac{n_{\ell}}{n} \frac{1}{n_{\ell}} \sum_{j \in \set{S}_{\ell}}
    \left[y^{(j)} - \widehat{g}^{(-\set{D}_{\ell})} \left(\vect{x}^{(j)}\right)\right]^2.

This implies:

.. math::

    \widehat{\operatorname{MSE}}_{KFold}
    & = \frac{1}{n} \sum_{\ell = 1}^k \sum_{j \in \set{S}_{\ell}}
    \left[y^{(j)} - \widehat{g}^{(-\set{D}_{\ell})} \left(\vect{x}^{(j)}\right)\right]^2.

Let :math:`\Delta^{(\ell, j)}` be defined as the K-Fold residual:

.. math::

    \Delta^{(\ell, j)}
    = y^{(j)} - \widehat{g}^{(-\set{D}_{\ell})} \left(\vect{x}^{(j)}\right)

for :math:`\ell = 1, ..., k` and :math:`j \in \set{S}_{\ell}`. Hence:

.. math::

    \widehat{\operatorname{MSE}}_{KFold}
    & = \frac{1}{n} \sum_{\ell = 1}^k \sum_{j \in \set{S}_{\ell}} \left(\Delta^{(\ell, j)}\right)^2 \\
    & = \frac{1}{n} \sum_{j = 1}^n \left(\Delta^{(\ell, j)}\right)^2

where :math:`\ell` is the index of the fold containing the :math:`j`-th observation.
The previous equation holds because the subsets :math:`\mathcal{S}_1, ..., \mathcal{S}_k` are, by hypothesis, a disjoint
partition of the full set of indices.
The previous equation states that the K-Fold mean squared error is the
sample mean of the corrected K-Fold squared residuals.

If the folds have equal sizes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Assume that the number of folds divides the sample size.
Mathematically, this means that :math:`k` divides :math:`n`.
In this special case, each fold has the same number of observations:

.. math::

    n_\ell = \frac{n}{k}

for :math:`\ell = 1, ..., k`. Hence all local K-Fold MSE have the
same weight:

.. math::

    \frac{n_\ell}{n} = \frac{1}{k}

for :math:`\ell = 1, ..., k`.
This implies that the K-Fold mean squared error has a  particularly simple expression
([deisenroth2020]_ eq. 8.13 page 264):

.. math::
  :label: kfoldMeanEqual

   \widehat{\operatorname{MSE}}_{KFold}
   = \frac{1}{k} \sum_{\ell = 1}^{k} \widehat{\operatorname{MSE}}^{(\ell)}

for :math:`\ell = 1, ..., k`.

Fast K-Fold cross-validation of a linear model
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this section, we present a fast version of the K-Fold cross-validation
that can be used for a linear regression model.
While evaluating the mean squared error with the fast LOO formula involves the division by :math:`1 - h_{jj}`,
using the fast K-Fold method involves the resolution of a linear system of equations.
The equations introduced by [shao1993]_ are presented in [suzuki2020]_ (proposition 14 page 71).

For any :math:`\ell \in \{1, ..., k\}`, let :math:`\boldsymbol{D}_{\ell} \in \Rset^{n_\ell \times m}`
be the rows of the design matrix :math:`\boldsymbol{D}` corresponding to the indices of the observations
involved in the :math:`\ell`-th fold:

.. math::

    \boldsymbol{D}_{\ell}
    =\begin{pmatrix}
    d_{j_1, 1} & \ldots & d_{j_1, m} \\
    \vdots & & \vdots \\
    d_{j_{n_\ell}, 1} & \ldots & d_{j_{n_\ell}, m}
    \end{pmatrix}

where :math:`j_1, ..., j_{n_\ell} \in \set{S}_{\ell}` are the
indices of the observations involved in the :math:`\ell`-th fold.
For any :math:`\ell \in \{1, ..., k\}`,
let :math:`\boldsymbol{H}_{\ell} \in \Rset^{n_{\ell} \times n_{\ell}}` be the sub-matrix of
the hat matrix corresponding to the indices of the observations in the
:math:`\ell`-th fold:

.. math::

    \boldsymbol{H}_{\ell}
    = \boldsymbol{D}_{\ell} \left(\boldsymbol{D}^T \boldsymbol{D} \right)^{-1} \boldsymbol{D}_{\ell}^T

It is not necessary to evaluate the previous expression in order to evaluate
the corresponding hat matrix.
Indeed, the matrix :math:`\boldsymbol{H}_{\ell}` can be computed by extracting the corresponding
rows and columns from the full hat matrix :math:`\boldsymbol{H}`:

.. math::

    \boldsymbol{H}_{\ell}
    =
    \begin{pmatrix}
    h_{j_1, j_1} & \ldots & h_{j_1, j_{n_\ell}} \\
    \vdots & & \vdots \\
    h_{j_{n_\ell}, j_1} & \ldots & h_{j_{n_\ell}, j_{n_\ell}}
    \end{pmatrix}.

Let :math:`\widehat{\boldsymbol{r}}_{\ell} \in \mathbb{R}^{n_{\ell}}` be the vector of
corrected K-Fold residuals:

.. math::

    (\boldsymbol{I}_{\ell} - \boldsymbol{H}_{\ell}) \widehat{\boldsymbol{r}}_{\ell}
    = \boldsymbol{y}_{\ell} - \widehat{\boldsymbol{y}}_{\ell}

where :math:`\boldsymbol{I}_{n_{\ell}} \in \Rset^{n_{\ell} \times n_{\ell}}` is the identity matrix,
:math:`\boldsymbol{y}_{\ell} \in \Rset^{n_{\ell}}` is the vector of output observations in the
:math:`\ell`-th fold:

.. math::

    \boldsymbol{y}_{\ell}
    = \left(y^{(j)}\right)^T_{j \in \set{S}_{\ell}},

and :math:`\widehat{\boldsymbol{y}}_{\ell} \in \Rset^{n_{\ell}}` is the corresponding
vector of output predictions from the linear least squares surrogate model:

.. math::

    \widehat{\boldsymbol{y}}_{\ell}
    = \left(g\left(\vect{x}^{(j)}\right)\right)^T_{j \in \set{S}_{\ell}}.

Then the mean squared error of the :math:`\ell`-th fold is:

.. math::

    \widehat{\operatorname{MSE}}^{(\ell)}
    = \frac{1}{n_{\ell}} \sum_{j \in \set{S}_{\ell}}
    \left(\widehat{\boldsymbol{r}}_{\ell}\right)_j^2.

Then the K-Fold mean squared error is evaluated from equation :eq:`kfoldMean`.

Conclusion
~~~~~~~~~~

The generic cross-validation method can be implemented using the following classes:

- :class:`~openturns.MetaModelValidation`: uses a test set to
  compute the mean squared error ;
- :class:`~openturns.LeaveOneOutSplitter`: uses the leave-one-out method
  to split the data set ;
- :class:`~openturns.KFoldSplitter`: uses the K-Fold method
  to split the data set.

Since the :class:`~openturns.LinearModelResult` is based on linear least
squares regression, fast methods are implemented in the :class:`~openturns.LinearModelValidation`.

See :ref:`pce_cross_validation` for specific methods for the the cross-validation
of a polynomial chaos expansion.

.. topic:: API:

    - See :class:`~openturns.MetaModelValidation`
    - See :class:`~openturns.LinearModelValidation`
    - See :class:`~openturns.KFoldSplitter`
    - See :class:`~openturns.LeaveOneOutSplitter`

.. topic:: References:

    - [blatman2009]_
    - [chapelle2002]_
    - [deisenroth2020]_
    - [stone1974]_
    - [shao1993]_
    - [suzuki2020]_
    - [wang2012]_

