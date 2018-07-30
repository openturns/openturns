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
  fucntion :math:`S`.

**The Cauchy spectral model**

Its is associated to the Exponential covariance model. The Cauchy spectral model is defined by:

.. math::
  :label: cauchyModel

    S_{ij}(f) = \displaystyle \frac{4R_{ij}a_ia_j(\lambda_i+ \lambda_j)}{(\lambda_i+ \lambda_j)^2 + (4\pi f)^2}, \quad \forall (i,j) \leq d

where :math:`\mat{R}`, :math:`\vect{a}` and :math:`\vect{\lambda}`
are the parameters of the Exponential covariance model defined in
section [ParamStationaryCovarianceFunction]. The relation
:eq:`cauchyModel` can be explicited with the spatial covariance function
:math:`\mat{C}^{spat}(\tau)` defined in :eq:`relRA`.

.. topic:: API:

    - See :class:`~openturns.CauchyModel`

.. topic:: Examples:

    - See :doc:`/examples/probabilistic_modeling/parametric_spectral_density`
