.. _code_calibration:

Code calibration
----------------

Introduction
~~~~~~~~~~~~

In this page, we present the method used in the :class:`~openturns.LinearLeastSquaresCalibration`
and :class:`~openturns.NonLinearLeastSquaresCalibration` classes.
We consider a computer model :math:`\vect{h}` (i.e. a deterministic function)
to calibrate:

.. math::

       \vect{z} = \vect{h}(\vect{x}, \vect{\theta}),

where

-  :math:`\vect{x} \in \Rset^{d_x}` is the input vector;

-  :math:`\vect{z} \in \Rset^{d_z}` is the output vector;

-  :math:`\vect{\theta} \in \Rset^{d_h}` are the unknown parameters of
   :math:`\vect{h}` to calibrate.

Let :math:`n \in \Nset` be the number of observations.
The standard hypothesis of the probabilistic calibration is:

.. math::

       \vect{Y}^i = \vect{z}^i + \vect{\varepsilon}^i,

for :math:`i=1,...,n` where :math:`\vect{\varepsilon}^i` is a random measurement error such that:

.. math::

       \Expect{\varepsilon} = \vect{0} \in \Rset^{d_z}, \qquad \Cov{\varepsilon} = \Sigma \in \Rset^{d_z\times d_z},

where :math:`\Sigma` is the error covariance matrix.

The goal of calibration is to estimate :math:`\vect{\theta}`, based on
observations of :math:`n` inputs :math:`(\vect{x}^1, \ldots, \vect{x}^n)`
and the associated :math:`n` observations of the output
:math:`(\vect{y}^1, \ldots, \vect{y}^n)`.
In other words, the calibration process reduces the discrepancy between
the observations :math:`(\vect{y}^1, \ldots, \vect{y}^n)` and the
predictions :math:`\vect{h}(\vect{\theta})`.
Given that :math:`(\vect{y}^1, \ldots, \vect{y}^n)` are realizations of a
random variable, the estimate of :math:`\vect{\theta}`, denoted by
:math:`\hat{\vect{\theta}}`, is also a random variable.
Hence, the secondary goal of calibration is to estimate the distribution of
:math:`\hat{\vect{\theta}}` representing the uncertainty of the calibration
process.

The standard observation model makes the hypothesis that the covariance matrix
of the error is diagonal, i.e.

.. math::

       \Sigma = \sigma^2 {\bf I}

where :math:`\sigma^2 \in \Rset` is the constant observation error variance.

In the remaining of this section, the input :math:`\vect{x}` is not involved
anymore in the equations.
This is why we simplify the equation into:

.. math::

       \vect{z} = \vect{h}(\vect{\theta}).

Least squares
~~~~~~~~~~~~~

The residuals is the difference between the observations and the predictions:

.. math::

       \vect{r}^i = \vect{y}^i - \vect{h}(\vect{\theta})^i

for :math:`i=1,...,n`.
The method of least squares minimizes the square of the euclidian norm
of the residuals.
This is why the least squares method is based on the cost function :math:`C` defined by:

.. math::

       c(\vect{\theta}) = \frac{1}{2} \|\vect{y} - \vect{h}(\vect{\theta})\|^2 = \frac{1}{2} \sum_{i=1}^n \left( \vect{y}^i - \vect{h}(\vect{\theta})^i \right)^2,

for any :math:`\vect{\theta} \in \Rset^{d_h}`.
The least squares method minimizes the cost function :math:`c`:

.. math::

       \hat{\vect{\theta}} = \argmin_{\vect{\theta} \in \Rset^{d_h}} \frac{1}{2} \|\vect{y} - \vect{h}(\vect{\theta})\|^2.

The unbiased estimator of the variance is:

.. math::

       \hat{\sigma}^2 = \frac{\|\vect{y} - \vect{h}(\vect{\theta})\|^2}{n - d_h}.

Notice that the previous estimator is not the maximum likelihood estimator (which is biased).

Linear least squares
~~~~~~~~~~~~~~~~~~~~

In the particular case where the deterministic function :math:`\vect{h}` is linear
with respect to the parameter :math:`\vect{\theta}`, then the method
reduces to the linear least squares.
Let :math:`J \in \Rset^{n \times d_h}` be the Jacobian matrix made of the
partial derivatives of :math:`\vect{h}` with respect to :math:`\vect{\theta}`:

.. math::

       J(\vect{\theta}) = \frac{\partial \vect{h}}{\partial \vect{\theta}}.

Let :math:`\vect{\mu} \in \Rset^{d_h}` be a reference value of the parameter :math:`\vect{\theta}`.
Let us denote by :math:`J=J(\vect{\mu})` the value of the Jacobian at the reference point :math:`\vect{\mu}`.
Since the function is, by hypothesis, linear, the Jacobian is independent of the
point where it is evaluated.
Since :math:`\vect{h}` is linear, it is equal to its Taylor expansion:

.. math::

       \vect{h}(\vect{\theta}) = \vect{h}(\vect{\mu}) + J (\vect{\theta} - \vect{\mu}),

for any :math:`\vect{\theta} \in \Rset^{d_h}`.
The corresponding linear least squares problem is:

.. math::

       \hat{\vect{\theta}} = \argmin_{\vect{\theta} \in \Rset^{d_h}} \frac{1}{2} \|\vect{y} - \vect{h}(\vect{\mu}) - J (\vect{\theta} - \vect{\mu})\|^2.

The Gauss-Markov theorem applied to this problem states that the solution is:

.. math::

       \hat{\vect{\theta}} = \vect{\mu} + \left(J^T J\right)^{-1} J^T ( \vect{y} - \vect{h}(\vect{\mu})).

The previous equations are the *normal equations*.
Notice, however, that the previous linear system of equations is not implemented as is,
i.e. we generally do not compute and invert the Gram matrix :math:`J^T J`.
Alternatively, various orthogonalization methods such as the QR or the SVD decomposition can
be used to solve the linear least squares problem so that potential ill-conditioning
of the normal equations is mitigated.

This estimator can be proved to be the best linear unbiased estimator, the *BLUE*, that is,
among the unbiased linear estimators, it is the one which minimizes the variance of the estimator.

Assume that the random observations are Gaussian:

.. math::

       \varepsilon \sim \mathcal{N}\left(\vect{0}, \; \sigma^2 {\bf I}\right).

Therefore, the distribution of :math:`\hat{\vect{\theta}}` is:

.. math::

       \hat{\vect{\theta}} \sim \mathcal{N}\left(\vect{\theta}, \; \sigma^2 J^T J\right).

The distribution of the estimator :math:`\hat{\vect{\theta}}` is the distribution
of the value of the parameters which best predicts the output, given the
variability in the observation generated by the random observation errors.

Non Linear Least squares
~~~~~~~~~~~~~~~~~~~~~~~~

In the general case where the function :math:`\vect{h}` is non linear
with respect to the parameter :math:`\vect{\theta}`, then the resolution
involves a non linear least squares optimization algorithm.
Instead of directly minimizing the squared Euclidian norm of the residuals,
most implementations rely on the residual vector, which lead to an improved accuracy.

One problem with non linear least squares is that, compared to the
linear situation, the theory does not provide the distribution
of :math:`\hat{\vect{\theta}}` anymore.
There are two practical solutions to overcome this limitation.

- bootstrap,

- linearization.

The bootstrap method is based on the following
experiment.
Provided that we can generate a set of input and output observations,
we can compute the corresponding value of the parameter :math:`\hat{\vect{\theta}}`.
Reproducing this sampling experiment a large number of times would allow one
to get the distribution of the estimated parameter :math:`\hat{\vect{\theta}}`.
In practice, we only have one single sample of :math:`n` observations.
If this sample is large enough and correctly represents the variability
of the observations, the bootstrap method allows one to generate
observations resamples, which, in turn, allow one to get a sample of
:math:`\hat{\vect{\theta}}`.
An approximate distribution of :math:`\hat{\vect{\theta}}` can then be computed
based on kernel smoothing, for example.
In order to get a relatively accurate distribution of :math:`\hat{\vect{\theta}}`, the
bootstrap sample size must be large enough.
Hence, this method requires to solve a number of optimization problems, which can be
time consuming.

Alternatively, we can linearize the function :math:`\vect{h}`
in the neighborhood of the solution :math:`\hat{\vect{\theta}}` and use the
Gaussian distribution associated with the linear least squares.
This method is efficient, but only accurate when the function :math:`\vect{h}`
is approximately linear with respect to :math:`\vect{\theta}` in the
neighborhood of :math:`\hat{\vect{\theta}}`.

Least squares and minimization of likelihood
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A link between the method of least squares and the method of maximum
likelihood can be done provided that two hypotheses are satisfied.
The first hypothesis is that the random output observations :math:`\vect{y}^i` are independent.
The second hypothesis is that the random measurement error :math:`\vect{\varepsilon}`
has the Gaussian distribution.
In this particular case, it can be shown that the solution of the least squares
problem maximizes the likelihood.
In other words, the least squares estimator is equivalent to the maximum
likelihood estimator.

This is the reason why, after a least squares calibration has been performed,
the distribution of the residuals may be interesting to analyze.
Indeed, if the distribution of the residuals is Gaussian and if the outputs
are independent, then the least squares estimator is the maximum likelihood estimator,
which gives a richer interpretation to the solution.
This validation can be done by visually comparing the distribution of the residuals
to the Gaussian distribution or by creating the QQ-Plot
against the Gaussian distribution (see :ref:`qqplot_graph`).

Regularization and ill-conditioned problems
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If a problem is ill-conditioned, a small change in the observations can
generate a large change in the estimate :math:`\hat{\vect{\theta}}`.
Hence, for problems which are ill-conditioned, calibration methods may include
some regularization features.

An ill-conditioned problem may appear in the particular case where the
Jacobian matrix :math:`J` is rank-degenerate.
For example, suppose that a linear least squares problem is considered,
where some linear combinations of the columns of :math:`J` are linearly dependent.
This implies that there is a linear subspace of the parameter space :math:`\hat{\vect{\theta}}`
such that linear combinations of the parameters do not have any
impact on the output.
In this case, it is not possible to estimate the projection of the solution on that
particular subspace.
Gaussian calibration is a way of mitigating this situation, by
constraining the solution to be *not too far away* from a reference solution,
named the *prior*.

.. topic:: API:

    - See :class:`~openturns.LinearLeastSquaresCalibration`
    - See :class:`~openturns.NonLinearLeastSquaresCalibration`

.. topic:: Examples:

    - See :doc:`/auto_calibration/least_squares_and_gaussian_calibration/plot_calibration_flooding`
    - See :doc:`/auto_calibration/least_squares_and_gaussian_calibration/plot_calibration_chaboche`
    - See :doc:`/auto_calibration/least_squares_and_gaussian_calibration/plot_calibration_deflection_tube`
    - See :doc:`/auto_calibration/least_squares_and_gaussian_calibration/plot_calibration_logistic`

.. topic:: References:

    - N. H. Bingham and John M. Fry (2010). *Regression, Linear Models in Statistics*, Springer Undergraduate Mathematics Series. Springer.
    - S. Huet, A. Bouvier, M.A. Poursat, and E. Jolivet (2004). *Statistical Tools for Nonlinear Regression*, Springer.
    - C. E. Rasmussen and C. K. I. Williams (2006), *Gaussian Processes for Machine Learning*, The MIT Press.

