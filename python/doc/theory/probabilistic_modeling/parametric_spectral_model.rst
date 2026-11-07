.. _parametric_spectral_model:

Parametric spectral density functions
-------------------------------------

Let :math:`X: \Omega \times \cD \rightarrow \Rset^{\processDim}` be a multivariate
stationary normal process of dimension :math:`\processDim`. We only treat here
the case where the domain is of dimension 1: :math:`\cD \subset \Rset`.
If the process is continuous, then :math:`\cD=\Rset`. In the discrete
case, :math:`\cD` is a lattice.
:math:`X` is supposed to be a second order process with zero mean and
we suppose that its spectral density function
:math:`S : \Rset \rightarrow \cH^+(\processDim)` defined in
:eq:`specdensFunc` exists.
:math:`\cH^+(\processDim) \in \cM_{\processDim}(\Cset)` is the set of
:math:`\processDim`-dimensional positive definite Hermitian matrices.
This page illustrates how to create a density spectral
function from parametric models. The library proposes the *Cauchy
spectral model* as a parametric model for the spectral density
function :math:`S`.

Example: the Cauchy spectral model
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is associated to the Kronecker covariance model built upon an exponential covariance model (AbsoluteExponential). The Cauchy spectral model is defined by:

.. math::
  :label: cauchyModel

    [S(f)]_{ij} = \displaystyle 2 \mat{\Sigma}_{ij}\prod_{k=1}^{\domainDim} \frac{\theta_k}{1 + (2\pi \theta_k f)^2}, \quad \forall (i,j) \leq \processDim

where :math:`\mat{\Sigma}` is the covariance matrix of the Kronecker
covariance model and :math:`\vect{\theta} = (\theta_1, \dots, \theta_{\domainDim})`
is the vector of scale parameters of the AbsoluteExponential covariance
model.

.. topic:: API:

    - See :class:`~openturns.CauchyModel`

.. topic:: Examples:

    - See :doc:`/auto_stochastic_processes/plot_parametric_spectral_density`
