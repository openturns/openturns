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
| In the mutlivariate case, we estimate the Box Cox transformation
  component by component and we define the multivariate Box Cox
  transformation as the aggregation of the marginal Box Cox
  transformations.

| **Marginal Box Cox tranformation:** 
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


where :math:`\lambda = 1-\frac{\beta}{2}`.

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

| **Estimation of the Box Cox tranformation:** 
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

    \hat{\beta}(\lambda) = \frac{1}{N} \sum_{k=0}^{N-1} h_{\lambda}(x_k) \\
    \hat{\sigma}^2(\lambda) = \frac{1}{N} \sum_{k=0}^{N-1} (h_{\lambda}(x_k) - \beta(\lambda))^2

| Substituting :eq:`eqBetaSigma` into :eq:`LKH` and taking the
  :math:`\log-`\ likelihood, we obtain:

  .. math::
    :label: lLambda

    \ell(\lambda) = \log L( \hat{\beta}(\lambda), \hat{\sigma}(\lambda),\lambda ) = C -
    \frac{N}{2}
    \log\left[\hat{\sigma}^2(\lambda)\right]
    \;+\;
    \left(\lambda - 1 \right) \sum_{k=0}^{N-1} \log(x_i)\,,

where :math:`C` is a constant.

The parameter :math:`\hat{\lambda}` is the one maximising :math:`\ell(\lambda)` defined in :eq:`lLambda`.

.. topic:: API:

    - See :class:`~openturns.BoxCoxTransform`
    - See :class:`~openturns.InverseBoxCoxTransform`
    - See :class:`~openturns.BoxCoxFactory`

.. topic:: Examples:

    - See :doc:`/examples/probabilistic_modeling/box_cox_transform`

