.. _sparse_gaussian_process_regression:

Sparse Gaussian process regression
----------------------------------

Introduction
~~~~~~~~~~~~

Gaussian process regression (see :any:`gaussian_process_regression`) requires the
factorisation of the covariance matrix discretized on the :math:`n` training inputs,
at a cost that scales as :math:`n^3`.
This cost becomes prohibitive when :math:`n` is large.

Sparse approximations introduce a set of :math:`m` inducing inputs with :math:`m \ll n`
and approximate the exact posterior by a variational distribution built on these
inducing inputs only.
The computational cost then scales as :math:`n \times m^2` instead of :math:`n^3`:
it is much reduced, but it still depends on the number of training samples :math:`n`
and it scales with the square of the number of inducing points :math:`m`.

Variational sparse approximation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We consider a Gaussian process :math:`Y` with covariance model :math:`k` and an
independent centered Gaussian noise of variance :math:`\sigma^2` on the observations
:math:`\vect{y} = (y_1, \dots, y_n)^t \in \Rset^n`.
Let :math:`p(y \mid f) = \cN(y \mid f, \sigma^2 \mat{I}_n)` denote the likelihood of
the observations given the latent function values :math:`\vect{f}` at the training
inputs :math:`\mat{X}`.

The model is approximated by introducing a set of :math:`m` inducing inputs
:math:`\mat{Z} = (\vect{z}_1, \dots, \vect{z}_m)^t` with :math:`m \leq n`.
The inducing inputs lie in the same input space as the training data.
They are associated with inducing variables :math:`\vect{u} = (u_1, \dots, u_m)^t`
corresponding to the evaluation of the Gaussian process at the inducing inputs, so that
:math:`p(\vect{u} \mid \mat{Z}) = \cN(\vect{u} \mid \vect{0}, \mat{K}_{uu})` with
:math:`\mat{K}_{uu} = k(\mat{Z}, \mat{Z})`.
The joint distribution of the latent function values :math:`\vect{f}` and the inducing
variables :math:`\vect{u}` is:

.. math::
    :label: sparseGPJoint

    p(\vect{f}, \vect{u} \mid \mat{X}, \mat{Z}) = \cN\left(
    \vect{0}, \begin{pmatrix} \mat{K}_{ff} & \mat{K}_{fu} \\ \mat{K}_{uf} & \mat{K}_{uu}
    \end{pmatrix}\right)

where :math:`\mat{K}_{ff} = k(\mat{X}, \mat{X})` and
:math:`\mat{K}_{fu} = k(\mat{X}, \mat{Z})`.
The conditional distribution of :math:`\vect{f}` given :math:`\vect{u}` is:

.. math::
    :label: sparseGPCond

    p(\vect{f} \mid \vect{u}, \mat{X}, \mat{Z}) = \cN(\vect{f} \mid
    \mat{K}_{fu} \mat{K}_{uu}^{-1} \vect{u},\, \mat{K}_{ff} - \mat{K}_{fu}
    \mat{K}_{uu}^{-1} \mat{K}_{fu}^t)

Collapsed evidence lower bound
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Following Titsias [titsias2009]_ (see also [gal2014]_ for the full derivations as
implemented in code and [leibfried2020]_ for the whitened parametrisation), the model
is approximated by a variational posterior in the whitened parametrisation:

.. math::
    :label: sparseGPPosterior

    q(\vect{w}) = \cN(\vect{m}_w, \mat{S}_{ww})

The optimal variational parameters are available in closed form given the covariance
model parameters, the inducing inputs and the noise standard deviation:

.. math::

    \vect{m}_w = \mat{B}^{-1} \mat{A}^t \vect{y}, \qquad
    \mat{S}_{ww} = \sigma^2 \mat{B}^{-1}

with :math:`\mat{A} = \mat{K}_{fu} \mat{L}_{uu}^{-t}`,
:math:`\mat{B} = \sigma^2 \mat{I}_m + \mat{A}^t \mat{A}`,
:math:`\mat{K}_{uu} = k(\mat{Z}, \mat{Z})` and :math:`\mat{L}_{uu}` its Cholesky factor.

The objective function, namely the collapsed Evidence Lower Bound (ELBO), is maximized
with respect to the active parameters of the covariance model, the logarithm of the
noise standard deviation and, optionally, the inducing inputs:

.. math::
    :label: sparseGPELBO

    \cL = -\frac{1}{2}\left(n\ln(2\pi) + (n - m)\ln(\sigma^2) + \ln\det(\mat{B})
    + \frac{\Vert \vect{y}\Vert^2 - \Vert \vect{c}\Vert^2}{\sigma^2}\right)
    - \frac{1}{2}\sum_{i=1}^n \ln\left(1 +
    \frac{k(\vect{x}_i, \vect{x}_i) - \vect{a}_i^t \vect{a}_i}{\sigma^2}\right)

where :math:`\vect{c} = \mat{L}_B^{-1} \mat{A}^t \vect{y}`,
:math:`\mat{L}_B` is the Cholesky factor of :math:`\mat{B}` and
:math:`\vect{a}_i^t` is the :math:`i`-th row of :math:`\mat{A}`.
The last sum is the tighter regularization of Titsias [titsias2025]_,
which replaces the trace term of the original bound.

When the number of inducing points :math:`m` equals the number of observations
:math:`n` and the inducing points equal the observations, the ELBO reduces to the
exact marginal log-likelihood of the Gaussian process model and the predictions
coincide with the ones of a :class:`~openturns.GaussianProcessRegression`.

Sparse prediction
~~~~~~~~~~~~~~~~~

Given the optimized inducing inputs :math:`\mat{Z}`, the variational mean
:math:`\vect{m}_w` and the Cholesky factor :math:`\mat{L}_{uu}`, the mean of the
prediction at a new point :math:`\vect{x}` is:

.. math::
    :label: sparseGPMean

    \mu(\vect{x}) = \vect{a}(\vect{x})^t \vect{m}_w

where :math:`\vect{a}(\vect{x}) = \mat{L}_{uu}^{-1} \vect{k}(\mat{Z}, \vect{x})` and
:math:`\vect{k}(\mat{Z}, \vect{x}) = (k(\vect{z}_1, \vect{x}), \dots,
k(\vect{z}_m, \vect{x}))^t` is the cross-covariance vector between the inducing
inputs and the prediction point.
The conditional variance :math:`\Var(Y(\vect{x}) \mid \vect{y})` quantifies the
predictive uncertainty.
The metamodel comes with its gradient and Hessian with respect to the input point.

.. topic:: API:

    - See :class:`~openturns.experimental.SparseGaussianProcessFitter`
    - See :class:`~openturns.experimental.SparseGaussianProcessRegression`
    - See :class:`~openturns.experimental.SparseGaussianProcessFitterResult`
    - See :class:`~openturns.experimental.SparseGaussianProcessEvaluation`
    - See :class:`~openturns.experimental.SparseGaussianProcessGradient`
    - See :class:`~openturns.experimental.SparseGaussianProcessHessian`


.. topic:: Examples:

    - See :doc:`/auto_surrogate_modeling/gaussian_process_regression/plot_gpr_cantilever_beam_sparse`


.. topic:: References:

    - [titsias2009]_
    - [gal2014]_
    - [leibfried2020]_
    - [titsias2025]_
