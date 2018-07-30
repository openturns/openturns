.. _estimate_non_stationary_covariance_model:

Estimation of a non stationary cov. model
=========================================

Let :math:`X: \Omega \times \cD \rightarrow \Rset^d` be a multivariate
normal process of dimension :math:`d` where :math:`\cD \in \Rset^n`.
:math:`X` is supposed to be a second order process and we note
:math:`C : \cD \times  \cD \rightarrow  \mathcal{M}_{d \times d}(\mathbb{R})`
its covariance function.
We denote :math:`(\vect{t}_0, \dots, \vect{t}_{N-1})` the vertices of
the common mesh :math:`\cM` and
:math:`(\vect{x}_0^k, \dots, \vect{x}_{N-1}^k)` the associated values
of the field :math:`k`. We suppose that we have :math:`K` fields.
We recall that the covariance function :math:`C` writes:

.. math::
  :label: covFunc

    \forall (\vect{s}, \vect{t}) \in \cD \times \cD, \quad C(\vect{s}, \vect{t}) = \Expect{\left(X_{\vect{s}}-m(\vect{s})\right)\left(X_{\vect{t}}-m(\vect{t})\right)^t}

where the mean function :math:`m: \cD \rightarrow \Rset^d` is defined by:

.. math::
  :label: meanFunc

    \forall \vect{t}\in \cD , \quad m(\vect{t}) = \Expect{X_{\vect{t}}}

First, we estimate the covariance function :math:`C` on the
vertices of the mesh :math:`\cM`. At each vertex
:math:`\vect{t}_i \in \cM`, we use the empirical mean estimator applied
to the :math:`K` fields to estimate:

1. :math:`m(\vect{t}_i)` at the vertex :math:`\vect{t}_i`:

.. math::
  :label: meanestim

    \displaystyle  \forall \vect{t}_i \in \cM, \quad m(\vect{t}_i) \simeq \frac{1}{K} \sum_{k=1}^{K} \vect{x}_i^k

2. :math:`C(\vect{t}_i, \vect{t}_j)` at the vertices
   :math:`(\vect{t}_i, \vect{t}_j)`:

.. math::
  :label: covEstm

      \displaystyle \forall (\vect{t}_i, \vect{t}_j) \in \cD \times \cD, \quad C(\vect{t}_i, \vect{t}_j) \simeq \frac{1}{K} \sum_{k=1}^{K} \left( \vect{x}_i^k -  m(\vect{t}_i) \right) \left( \vect{x}_j^k -  m(\vect{t}_j) \right)^t

Then, we build a covariance function defined on
:math:`\cD \times \cD` which is a piecewise constant function defined
on :math:`\cD \times \cD` by:

.. math::

    \begin{aligned}
       \forall (\vect{s}, \vect{t}) \in \cD \times \cD, \, C^{stat}(\vect{s}, \vect{t}) =  C(\vect{t}_k, \vect{t}_l)\end{aligned}

where :math:`k` is such that :math:`\vect{t}_k` is the vertex of
:math:`\cM` the nearest to :math:`\vect{s}` and :math:`\vect{t}_l` the
nearest to :math:`\vect{t}`.

.. topic:: API:

    - See :class:`~openturns.NonStationaryCovarianceModelFactory`
    - See :class:`~openturns.UserDefinedCovarianceModel`

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/estimate_non_stationary_covariance_model`
