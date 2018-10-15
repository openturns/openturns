.. _cross_validation:

Cross validation assessment of PC models
----------------------------------------

| Once a polynomial response surface :math:`\widehat{h}(\underline{x})`
  of the original numerical model :math:`h(\underline{x})` has been
  built up, it is crucial to estimate the approximation error, i.e. the
  discrepancy between the response surface and the true model response
  in terms of a suitable norm such as the :math:`L_2`-norm:

  .. math:: Err \, \, = \, \, \left\| h(\underline{x}) \; - \; \widehat{h}(\underline{x}) \right\|_{L_2}^2\, \, = \, \, \int_{\cD} \; \left( h(\underline{x}) \; - \; \widehat{h}(\underline{x}) \right)^2  \; d\underline{x}

  where :math:`\cD` denotes the support of the input parameters
  :math:`\underline{x}`. It is worth emphasizing that one tends to
  overestimate the performance of a response surface by training and
  evaluating it on the same data set. For instance, the model might fail
  to predict on new data whereas the validation on the training data
  yields satisfactory performance. To avoid such issue, it is important
  that the model error assessment is conducted on a data set which is
  independent of the training sample. However, any new model evaluation
  may be time- and memory-consuming. Therefore, error estimates which
  are only based on already performed computer experiments are of
  interest. In this context, the so-called *cross validation* techniques
  are utilized to obtain reliable performance assessment of the response
  surface without additional model evaluations.

Any cross-validation scheme consists in dividing the data sample (i.e.
the experimental design) into two sub-samples that are independently and
identically distributed. A metamodel :math:`\widehat{h}(\underline{x})`
is built from one sub-sample, i.e. the *training set*, and its
performance is assessed by comparing its predictions to the other
subset, i.e. the *test set*. A single split will lead to a validation
estimate. When several splits are conducted, the cross-validation error
estimate is obtained by averaging over the splits.

**K-fold cross-validation error estimate**

| The :math:`K`-fold cross-validation technique relies on splitting the
  data set :math:`\cX` into :math:`K` mutually exclusive sub-samples
  :math:`\cX_1, \dots, \cX_K` of approximately equal size. A sub-sample
  :math:`\cX_i`, :math:`i \in \{ 1, \dots, K\}` is set aside, then the
  response surface :math:`\widehat{h}^{(-\cX_i)}` is built on the
  remaining sub-samples :math:`\cX \setminus \cX_i`. The approximation
  error is estimated on the set-aside sample :math:`\cX_i`:

  .. math:: Err^{(i)}  = \dfrac{1}{ |\cX_i|}  \sum\limits_{\underline{x}^{(j)} \in \cX_i} \left[ h(\underline{x}^{(j)}) - \widehat{h}^{(-\cX_i)} {(\underline{x}^{(j)})} \right]^2

  in which
  :math:`h(\underline{x}^{(j)}) - \widehat{h}^{(-\cX_i)} {(\underline{x}^{(j)})}`
  is the *predicted residual* defined as the difference between the
  evaluation of :math:`h(\cdot)` and the prediction with
  :math:`\widehat{h}^{(-\cX_i)}(\cdot)` at :math:`\underline{x}^{(j)}`
  in the sub-sample :math:`\cX_i` whose cardinality is :math:`|\cX_i|`.

| The approximation errors :math:`Err^{(i)}` are estimated with each of
  the sub-samples :math:`\cX_i`, :math:`i \in \{ 1, \dots, K\}` being
  used as the validation set whereas the remaining sub-samples
  :math:`\cX \setminus \cX_i` being used for training. Finally the
  :math:`K`-fold cross-validation error estimate is obtained as the
  average:

  .. math:: Err_{Kfold} = \dfrac{1}{K} \sum\limits_{i=1}^{K} Err^{(i)}

  As described above, the :math:`K`-fold error estimate can be obtained
  with a single split of the data :math:`\cX` into :math:`K` folds. It
  is worth noting that one can repeat the cross-validation multiple
  times using different divisions into folds to obtain better Monte
  Carlo estimate. This comes obviously with an additional computational
  cost.

**Classical leave-one-out error estimate**

| The *leave-one-out* (LOO) cross-validation is a special case of
  :math:`K`-fold cross-validation where the number of folds :math:`K` is
  chosen equal to the cardinality :math:`N` of the experimental design
  :math:`\cX`. It is recalled that a :math:`P`-term polynomial
  approximation of the model response reads:

  .. math:: \widehat{h}(\underline{x}) \, \, = \, \,  \sum_{j=0}^{P-1} \; \widehat{a}_{j} \; \psi_{j}(\underline{x})

  where the :math:`\widehat{a}_{j}`\ ’s are estimates of the
  coefficients obtained by a specific method, e.g. least squares.

| Let us denote by :math:`\widehat{h}^{(-i)}` the approximation that has
  been built from the experimental design
  :math:`\cX \setminus \{\underline{x}^{(i)}\}` with the :math:`i`-th
  observation :math:`\underline{x}^{(i)}` being set aside. The predicted
  residual is defined as the difference between the model evaluation at
  :math:`\underline{x}^{(i)}` and its prediction based on
  :math:`\widehat{h}^{(-i)}`:

  .. math::
    :label: 4.3:5

      \Delta^{(i)} \, \, = \, \,  h(\underline{x}^{(i)}) - \widehat{h}^{(-i)}(\underline{x}^{(i)})

  By repeating this process for all observations in the experimental
  design, one obtains the predicted residuals
  :math:`\Delta^{(i)}, i = 1, \dots , N`. Finally, the LOO error is
  estimated as follows:

  .. math::
    :label: 4.3:6

      Err_{LOO} \, \, = \, \, \frac{1}{N} \sum_{i=1}^{N} {\Delta^{(i)}}^{2}

  Due to the linear-in-parameters form of the polynomial chaos
  expansion, the quantity :math:`Err_{LOO}` may be computed without
  performing further regression calculations when the PC coefficients
  have been estimated using the entire experimental design :math:`\cX`.
  Indeed, the predicted residuals can be obtained analytically as
  follows:

  .. math::
    :label: 4.3:7

      \Delta^{(i)} \, = \,
         \frac{h(\underline{x}^{(i)}) - \widehat{h}(\underline{x}^{(i)})}{1 - h_i}

  where :math:`h_i` is the :math:`i`-th diagonal term of the matrix
  :math:`\underline{\underline{\Psi}} (\underline{\underline{\Psi}}^{\textsf{T}}\underline{\underline{\Psi}})^{-1} \underline{\underline{\Psi}}^{\textsf{T}}`
  with :math:`\underline{\underline{\Psi}}` being the information
  matrix:

  .. math::
    :label: 4.3:7bis

      \underline{\underline{\Psi}} \, \, = \, \, \left\{ \psi_{j}(\underline{x}^{(i)}) \; , \; i=1,\dots,N \; , \; j = 0,\dots,P-1 \right\}

  In practice, one often computes the following normalized LOO error:

  .. math::
    :label: 4.3:8bis

      \varepsilon_{LOO} \, \, \equiv \, \, \frac{Err_{LOO}}{\hat{\Cov{\cY}}}

  where :math:`\hat{\Cov{\mathcal{Y}}}` denotes the empirical
  covariance of the response sample :math:`\cY`:

  .. math::
    :label: 4.3:4bis

      \hat{\Cov{\mathcal{Y}}} \, \, \equiv \, \, \frac{1}{N-1} \; \sum_{i=1}^{N} \; \left( y^{(i)} \; - \;   \bar{\mathcal{Y}}  \right)^{2} \quad  \quad , \quad \quad   \bar{\mathcal{Y}} \, \, \equiv \, \, \frac{1}{N} \; \sum_{i=1}^{N} \; y^{(i)}

**Corrected leave-one-out error estimate**

A penalized variant of :math:`\varepsilon_{LOO}` may be used in order to
increase its robustness with respect to overfitting, i.e. to penalize a
large number of terms in the PC expansion compared to the size of the
experimental design:

.. math:: \varepsilon_{LOO}^{*} \, \, \equiv \, \, \varepsilon_{LOO} \, T(P,N)

The penalty factor is defined by:

.. math:: T(P,N) \, \, = \, \,   \frac{N}{N-P}  \; \left(1 \; + \; \frac{\mbox{tr} \left( \underline{\underline{C}}_{emp}^{-1}  \right) }{N} \right)

where:

.. math::
  :label: 4.3:10bis

    \underline{\underline{C}}_{emp} \, \, \equiv \, \, \frac{1}{N}\underline{\underline{\Psi}}^{\textsf{T}}\; \underline{\underline{\Psi}} \quad \quad , \quad \quad
    \underline{\underline{\Psi}} \, \, = \, \, \left\{ \psi_{j}(\underline{x}^{(i)}) \, \, , \, \, i=1,\dots,N \, \, , \, \, j=0,\dots,P-1 \right\}


Leave-one-out cross validation is also known as jackknife in statistics.

.. topic:: API:

    - See :class:`~openturns.MetaModelValidation`


.. .. topic:: Examples:
.. 
..     - See :doc:`/examples/data_analysis/XXXXXXXXXXXXx`


.. topic:: References:

    - [blatman2009]_

