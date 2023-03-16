.. _boxcox_transformation:

Box Cox transformation
======================

| We consider :math:`X: \Omega \times \cD \rightarrow \Rset^d` a
  multivariate stochastic process of dimension :math:`d` where
  :math:`\cD \in \Rset^n` and :math:`\omega \in \Omega` is an event. We
  suppose that the process is :math:`\cL^2(\Omega)`.
| We note :math:`X_{\vect{t}}: \Omega \rightarrow \Rset^d` the random
  variable at the vertex :math:`\vect{t} \in \cD` defined by
  :math:`X_{\vect{t}}(\omega)=X(\omega, \vect{t})`.
| If the variance of :math:`X_{\vect{t}}` depends on the vertex
  :math:`\vect{t}`, the Box Cox transformation maps the process
  :math:`X` into the process :math:`Y` such that the variance of
  :math:`Y_{\vect{t}}` is constant (at the first order at least) with
  respect to :math:`\vect{t}`.
| We present here:

-  the estimation of the Box Cox transformation from a given field of
   the process :math:`X`,

-  the action of the Box Cox transformation on a field generated from
   :math:`X`.

| We note :math:`h: \Rset^d \rightarrow \Rset^d` the Box Cox
  transformation which maps the process :math:`X` into the process
  :math:`Y: \Omega \times \cD \rightarrow \Rset^d`, where
  :math:`Y=h(X)`, such that :math:`\Var{Y_{\vect{t}}}` is independent of
  :math:`\vect{t}` at the first order.
| We suppose that :math:`X_{\vect{t}}` is a positive random variable for
  any :math:`\vect{t}`. To verify that constraint, it may be needed to
  consider the shifted process :math:`X+\vect{\alpha}`.
| We illustrate some usual Box Cox transformations :math:`h` in the
  scalar case (:math:`d`\ =1), using the Taylor development of
  :math:`h: \Rset \rightarrow \Rset` at the mean point of
  :math:`X_{\vect{t}}`.
| In the multivariate case, we estimate the Box Cox transformation
  component by component and we define the multivariate Box Cox
  transformation as the aggregation of the marginal Box Cox
  transformations.

| **Marginal Box Cox transformation:**
| The first order Taylor development of :math:`h` around
  :math:`\Expect{Y_{\vect{t}}}` writes:

  .. math::

      \forall \vect{t} \in \cD, h(X_{\vect{t}}) = h(\Expect{X_{\vect{t}}}) + (X_{\vect{t}} - \Expect{X_{\vect{t}}})h'(\Expect{X_{\vect{t}}})

which leads to:

  .. math::

      \Expect{h(X_{\vect{t}})} = h(\Expect{X_{\vect{t}}})

and then:

  .. math::

      \Var{h(X_{\vect{t}})} = h'(\Expect{X_{\vect{t}}})^2  \Var{X_{\vect{t}}}

To have :math:`\Var{h(X_{\vect{t}})}` constant with respect to
:math:`\vect{t}` at the first order, we need:

.. math::
  :label: eqh

    h'(\Expect{X_{\vect{t}}}) = k \left(  \Var{X_{\vect{t}}} \right)^{-1/2}

Now, we make some additional hypotheses on the relation between
:math:`\Expect{X_{\vect{t}}}` and :math:`\Var{X_{\vect{t}}}`:

-  If we suppose that
   :math:`\Var{X_{\vect{t}}} \propto \Expect{X_{\vect{t}}}`, then
   :eq:`eqh` leads to the function :math:`h(y) \propto \sqrt{y}` and we
   take :math:`h(y) = \sqrt{y}, y~>~0`;

-  If we suppose that
   :math:`\Var{X_{\vect{t}}} \propto (\Expect{X_{\vect{t}}})^2` , then
   :eq:`eqh` leads to the function :math:`h(y) \propto \log{y}` and we
   take :math:`h(y) = \log{y}, y>0`;

-  More generally, if we suppose that
   :math:`\Var{X_{\vect{t}}} \propto (\Expect{X_{\vect{t}}})^{\beta}`,
   then :eq:`eqh` leads to the function :math:`h_\lambda` parametrized by
   the scalar :math:`\lambda`:

   .. math::
    :label: BoxCoxModel

        h_\lambda(y) =
        \left\{
        \begin{array}{ll}
          \frac{y^\lambda-1}{\lambda} & \lambda \neq 0 \\
          \log(y)                     & \lambda = 0
        \end{array}
        \right.


The inverse Box Cox transformation is defined by:

.. math::
  :label: InverseBoxCoxModel

     h^{-1}_\lambda(y) =
     \left\{
     \begin{array}{ll}
       \displaystyle (\lambda y + 1)^{\frac{1}{\lambda}} & \lambda \neq 0 \\
       \displaystyle \exp(y)                          & \lambda = 0
     \end{array}
     \right.

| **Estimation of the Box Cox transformation:**
| The parameter :math:`\lambda` is estimated from a given field of the
  process :math:`X` as follows.
| The estimation of :math:`\lambda` given below is optimized in the case
  when :math:`h_\lambda(X_{\vect{t}}) \sim \cN(\beta , \sigma^2 )` at
  each vertex :math:`\vect{t}`. If it is not the case, that estimation
  can be considered as a proposition, with no guarantee.
| The parameters :math:`(\beta,\sigma,\lambda)` are then estimated by
  the maximum likelihood estimators. We note
  :math:`\Phi_{\beta, \sigma}` and :math:`\phi_{\beta, \sigma}`
  respectively the cumulative distribution function and the density
  probability function of the :math:`\cN(\beta , \sigma^2)`
  distribution.
| For all vertices :math:`\vect{t}`, we have:

  .. math::
    :label: cdfYt

      \forall v \geq 0, \, \Prob{ X_{\vect{t}} \leq v } = \Prob{ h_\lambda(X_{\vect{t}}) \leq h_\lambda(v) } \\
      = \Phi_{\beta, \sigma} \left(h_\lambda(v)\right)

from which we derive the density probability function :math:`p` of
:math:`X_{\vect{t}}` for all vertices :math:`\vect{t}`:

.. math::
  :label: pdfYt

    p(v) = h_\lambda'(v)\phi_{\beta, \sigma}(v) = v^{\lambda - 1}\phi_{\beta, \sigma}(v)

Using :eq:`pdfYt`, the likelihood of the values
:math:`(x_0, \dots, x_{N-1})` with respect to the model :eq:`cdfYt`
writes:

.. math::
  :label: LKH

     L(\beta,\sigma,\lambda) =
     \underbrace{ \frac{1}{(2\pi)^{N/2}}
       \times
       \frac{1}{(\sigma^2)^{N/2}}
       \times
       \exp\left[
         -\frac{1}{2\sigma^2}
         \sum_{k=0}^{N-1}
         \left(
         h_\lambda(x_k)-\beta
         \right)^2
         \right]
     }_{\Psi(\beta, \sigma)}
     \times
     \prod_{k=0}^{N-1} x_k^{\lambda - 1}

We notice that for each fixed :math:`\lambda`, the likelihood equation
is proportional to the likelihood equation which estimates
:math:`(\beta, \sigma^2)`. Thus, the maximum likelihood estimator for
:math:`(\beta(\lambda), \sigma^2(\lambda))` for a given :math:`\lambda`
are:

.. math::
  :label: eqBetaSigma

    \hat{\beta}(\lambda) = \frac{1}{N} \sum_{k=1}^{N} h_{\lambda}(x_k) \\
    \hat{\sigma}^2(\lambda) = \frac{1}{N} \sum_{k=1}^{N} (h_{\lambda}(x_k) - \beta(\lambda))^2

| Substituting :eq:`eqBetaSigma` into :eq:`LKH` and taking the
  :math:`\log-`\ likelihood, we obtain:

  .. math::
    :label: lLambda

    \ell(\lambda) = \log L( \hat{\beta}(\lambda), \hat{\sigma}(\lambda),\lambda ) = C -
    \frac{N}{2}
    \log\left[\hat{\sigma}^2(\lambda)\right]
    \;+\;
    \left(\lambda - 1 \right) \sum_{k=1}^{N} \log(x_k)\,,

where :math:`C` is a constant.

The parameter :math:`\hat{\lambda}` is the one maximizing :math:`\ell(\lambda)` defined in :eq:`lLambda`.

| **Estimation of the Box Cox transformation in the frame of general linear model:**

In the frame of the general linear model, we consider a functional relation between some input and
output values. Let us consider the following dataset:
:math:`\left(\left(\vect{x}^{(i)}, y^{(i)}\right), i = 1, \ldots, m\right)`.

The general linear model aims at assessing the following
prior model :

.. math:: Y(\vect{x}) = \Tr{\vect{\phi}(\vect{x})} \vect{\alpha} + Z(\vect{x})

where:

-  :math:`\Tr{\vect{\phi}(\vect{x})} \vect{\alpha}` is a general linear
   model based upon a functional basis
   :math:`\vect{\phi} = \left(\phi_j, j = 1, \ldots, p\right)` and a vector of
   coefficients
   :math:`\vect{\alpha} = \left(\alpha_j, j = 1, \ldots, p\right)`,

-  :math:`Z` is a zero-mean stationary Gaussian process whose covariance
   function reads:

   .. math:: \mathbb{E}[Z(\vect{x})\,Z(\vect{x'})] = \tau^2 R(\vect{x} - \vect{x'}, \vect{\theta})

   where :math:`\tau^2 > 0` is the variance and :math:`R` is the
   correlation function that solely depends on the Manhattan distance
   between input points :math:`\vect{x} - \vect{x'}` and a vector of
   parameters :math:`\vect{\theta} \in \Rset^{n_\theta}`.

The optimal parameters of such model are estimated by maximizing a log-likelihood function.

Here we suppose a gaussian prior on :math:`h_\lambda(y)`. Thus, if we write our various hypotheses,
we get the following log-likelihood function to be optimized:

  .. math::
    :label: lLambdaglm

    \ell(\lambda) = \log L( \hat{\beta}(\lambda), \hat{\sigma}(\lambda),\lambda ) = C -
    \frac{m}{2}
    \log\left[\hat{\sigma}^2(\lambda)\right]
    \;+\;
    \left(\lambda - 1 \right) \sum_{k=1}^{m} \log(y_k)\,,

where :math:`C` is a constant,

   .. math::
    :label: betasigmaglm

    \epsilon_k = y_k - \Tr{\vect{\phi}(\vect{x_k})} \vect{\alpha}, k=1...m \\
    \hat{\beta}(\lambda) = \frac{1}{m} \sum_{k=1}^{m} h_{\lambda}(\epsilon_k) \\
    \hat{\sigma}^2(\lambda) = \frac{1}{m} \sum_{k=1}^{m} (h_{\lambda}(\epsilon_k) - \beta(\lambda))^2

Remarks :

-  The equation :eq:`lLambdaglm` applies also if we replace the general linear model by a linear regression model.
   Indeed a linear model is a specific case of general linear model where the correlation model is a
   Dirac covariance model.

Note that such estimate might be heavy as we get a double loop optimization. Indeed for each :math:`\lambda` value, we optimize
the parameters of the underlying general linear model. Some practitioners are used to freeze the first general linear model parameters
and then preform a one loop optimization selecting only the best :math:`\lambda` value.

| **Estimation of the Box Cox transformation in the frame of linear models:**

In the frame of linear models, we consider a functional relation between some input and
output values. Let us consider the following dataset:
:math:`\left(\left(\vect{x}^{(i)}, y^{(i)}\right), i = 1, \ldots, m\right)`.

The general linear model aims at assessing the following
prior model :

.. math:: Y(\vect{x}) = \Tr{\vect{\phi}(\vect{x})} \vect{\alpha} + \epsilon

where:

-  :math:`\Tr{\vect{\phi}(\vect{x})} \vect{\alpha}` is a general linear
   model based upon a functional basis
   :math:`\vect{\phi} = \left(\phi_j, j = 1, \ldots, p\right)` and a vector of
   coefficients
   :math:`\vect{\alpha} = \left(\alpha_j, j = 1, \ldots, p\right)`,

-  :math:`\epsilon` is a zero-mean stationary white noise process.


The optimal parameters of such model are estimated by maximizing a log-likelihood function.

Here we suppose a gaussian prior on :math:`h_\lambda(y)`. Thus, if we write our various hypotheses,
we get the following log-likelihood function to be optimized:

  .. math::
    :label: lLambdalm

    \ell(\lambda) = \log L( \hat{\beta}(\lambda), \hat{\sigma}(\lambda),\lambda ) = C -
    \frac{m}{2}
    \log\left[\hat{\sigma}^2(\lambda)\right]
    \;+\;
    \left(\lambda - 1 \right) \sum_{k=1}^{m} \log(y_k)\,,

where :math:`C` is a constant,

   .. math::
    :label: betasigmalm

    \epsilon_k = y_k - \Tr{\vect{\phi}(\vect{x_k})} \vect{\alpha}, k=1...m \\
    \hat{\beta}(\lambda) = \frac{1}{m} \sum_{k=1}^{m} h_{\lambda}(\epsilon_k) \\
    \hat{\sigma}^2(\lambda) = \frac{1}{m} \sum_{k=1}^{m} (h_{\lambda}(\epsilon_k) - \beta(\lambda))^2

As a remark, the above case is a particular case of :eq:`lLambdalm`. Indeed if a linear model is a specific case of general linear model
where the correlation model is a Dirac covariance model (White noise model).

In term of costs, a factorization (QR or SVD) is done once for the regression matrix and the parameters defined in :eq:`betasigmalm`
are easily obtained, for each new :math:`\lambda` value, solving the corresponding linear systems.
Sometimes, people perform a grid search for example varying for example :math:`\lambda` from -3 to 3 using a small step. It allows one
to get both the optimal and assess the robustness of the optimum.

.. topic:: API:

    - See :class:`~openturns.BoxCoxTransform`
    - See :class:`~openturns.InverseBoxCoxTransform`
    - See :class:`~openturns.BoxCoxFactory`
    - See :class:`~openturns.GeneralLinearModelAlgorithm`
    - See :class:`~openturns.LinearModelAlgorithm`

.. topic:: Examples:

    - See :doc:`/auto_probabilistic_modeling/stochastic_processes/plot_box_cox_transform`

