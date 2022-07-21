.. _parametric_spectral_model:

Parametric spectral density functions
=====================================

| Let :math:`X: \Omega \times \cD \rightarrow \Rset^d` be a multivariate
  stationary normal process of dimension :math:`d`. We only treat here
  the case where the domain is of dimension 1: :math:`\cD \in \Rset`
  (:math:`n=1`).
| If the process is continuous, then :math:`\cD=\Rset`. In the discrete
  case, :math:`\cD` is a lattice.
| :math:`X` is supposed to be a second order process with zero mean and
  we suppose that its spectral density function
  :math:`S : \Rset \rightarrow \mathcal{H}^+(d)` defined in
  :eq:`specdensFunc` exists.
  :math:`\mathcal{H}^+(d) \in \mathcal{M}_d(\Cset)` is the set of
  :math:`d`-dimensional positive definite hermitian matrices.
| This use case illustrates how the User can create a density spectral
  function from parametric models. The library proposes the *Cauchy
  spectral model* as a parametric model for the spectral density
  function :math:`S`.

**The Cauchy spectral model**

It is associated to the Kronecker covariance model built upon an exponential covariance model (AbsoluteExponential). The Cauchy spectral model is defined by:

.. math::
  :label: cauchyModel

    [S(f)]_{ij} = \displaystyle 2 \mat{\Sigma}_{ij}\prod_{k=1}^{n} \frac{\theta_k}{1 + (2\pi \theta_k f)^2}, \quad \forall (i,j) \leq d

where :math:`\mat{\Sigma}` is the covariance matrix of the Kronecker
covariance model and :math:`\vect{\theta} = (\theta_1, \dots, \theta_n)`
is the vector of scale parameters of the AbsoluteExponential covariance 
model.

.. topic:: API:

    - See :class:`~openturns.CauchyModel`

.. topic:: Examples:

    - See :doc:`/auto_probabilistic_modeling/stochastic_processes/plot_parametric_spectral_density`
