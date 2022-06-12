.. _stationary_covariance_model:

Parametric stationary covariance models
=======================================

| Let :math:`X: \Omega \times \cD \rightarrow \Rset^d` be a multivariate
  stationary normal process where :math:`\cD \in \Rset^n`. The process
  is supposed to be zero mean. It is entirely defined by its covariance
  function
  :math:`C^{stat}: \cD \rightarrow  \mathcal{M}_{d \times d}(\Rset)`,
  defined by
  :math:`C^{stat}(\vect{\tau})=\Expect{X_{\vect{s}}X_{\vect{s}+\vect{\tau}}^t}`
  for all :math:`\vect{s}\in \Rset^n`.
| If the process is continuous, then :math:`\cD=\Rset^n`. In the
  discrete case, :math:`\cD` is a lattice.

| This use case highlights how User can create a covariance
  function from parametric models. The library proposes many parametric
  covariance models. The *multivariate Exponential model* is one of them.
   :math:`C^{stat}`.

**The multivariate exponential model**

This model defines the covariance function :math:`C^{stat}` by:

.. math::
  :label: fullMultivariateExponential2

    \forall \vect{\tau} \in \cD,\quad C^{stat}( \vect{\tau} )= \rho\left(\dfrac{\vect{\tau}}{\theta}\right)\, \mat{C^{stat}}(\vect{\tau})

where the correlation function :math:`\rho` is given by:

.. math::
  :label: rhoExponentialModel

    \rho(\vect{\tau} ) = e^{-\left\| \vect{\tau} \right\|_2} \quad \forall (\vect{s}, \vect{t}) \in \cD

and the spatial covariance matrix :math:`\mat{C^{stat}}(\vect{s}, \vect{t})` by:

.. math::
  :label: cstat_exp_model

   \mat{C^{stat}}(\vect{\tau})= \mbox{Diag}(\vect{\sigma}) \, \mat{R} \,  \mbox{Diag}(\vect{\sigma}).

with :math:`\mat{R} \in \mathcal{M}_{d \times d}([-1, 1])` a correlation matrix
, :math:`\theta_i>0` and :math:`\sigma_i>0` for any :math:`i`.

The expression of :math:`C^{stat}` is the combination of:

-  the matrix :math:`\mat{R}` that models the spatial correlation
   between the components of the process :math:`X` at any vertex
   :math:`\vect{t}` (since the process is stationary):

   .. math::
     :label: fullMultivariateExponential1

      \forall \vect{t}\in \cD,\quad \mat{R} = \Cor{X_{\vect{t}}, X_{\vect{t}}}

-  the matrix :math:`\mbox{Diag}(\vect{\sigma})` that models the variance of each marginal
   random variable:

   .. math::

      \begin{aligned}
          \Var{X_{\vect{t}}} = (\sigma_1, \dots, \sigma_d)
        \end{aligned}

It is possible to define the exponential model from the spatial
covariance matrix :math:`\mat{C}^{spat}` rather than the correlation
matrix :math:`\mat{R}` :

.. math::
  :label: relRA

    \forall \vect{t} \in \cD,\quad \mat{C}^{spat} = \Expect{X_{\vect{t}}X^t_{\vect{t}}} = \mbox{Diag}(\vect{\sigma})\,\mat{R}\, \mbox{Diag}(\vect{\sigma})

.. topic:: API:

    - See :class:`~openturns.AbsoluteExponential`
    - See :class:`~openturns.DiracCovarianceModel`
    - See :class:`~openturns.ExponentialModel`
    - See :class:`~openturns.KroneckerCovarianceModel`
    - See :class:`~openturns.ExponentiallyDampedCosineModel`
    - See :class:`~openturns.GeneralizedExponential`
    - See :class:`~openturns.MaternModel`
    - See :class:`~openturns.SquaredExponential`

.. topic:: Examples:

    - See :doc:`/auto_probabilistic_modeling/stochastic_processes/plot_create_stationary_covmodel`
    - See :doc:`/auto_probabilistic_modeling/stochastic_processes/plot_user_stationary_covmodel`
