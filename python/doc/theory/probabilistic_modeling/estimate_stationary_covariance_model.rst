.. _estimate_stationary_covariance_model:

Estimation of a stationary covariance model
===========================================

Let :math:`X: \Omega \times \cD \rightarrow \Rset^d` be a multivariate
stationary normal process of dimension :math:`d`. We only treat here
the case where the domain is of dimension 1: :math:`\cD \in \Rset`
(:math:`n=1`).
If the process is continuous, then :math:`\cD=\Rset`. In the discrete
case, :math:`\cD` is a lattice.
:math:`X` is supposed a second order process with zero mean. It is
entirely defined by its covariance function
:math:`C^{stat}:  \cD \rightarrow  \mathcal{M}_{d \times d}(\Rset)`,
defined by :math:`C^{stat}(\tau)=\Expect{X_sX_{s+\tau}^t}` for all
:math:`s\in \cD`.
In addition, we suppose that its spectral density function
:math:`S : \Rset \rightarrow \mathcal{H}^+(d)` is defined, where
:math:`\mathcal{H}^+(d) \in \mathcal{M}_d(\Cset)` is the set of
:math:`d`-dimensional positive definite hermitian matrices.
The objective is to estimate :math:`C^{stat}` from a
field or a sample of fields from the process :math:`X`, using first
the estimation of the spectral density function and then mapping
:math:`S` into :math:`C^{stat}` using the inversion relation
:eq:`cspectransform`, when it is possible.
As the mesh is a time grid (:math:`n=1`), the fields can be
interpreted as time series.
The estimation algorithm is outlined hereafter.
Let :math:`(t_{0},t_{1},\hdots t_{N-1})` be the time grid on which the
process is observed and let also
:math:`(\vect{X}^0, \dots, , \vect{X}^{M-1})` be :math:`M` independent
realizations of :math:`X` or :math:`M` segments of one realization of
the process.
Using :eq:`cspectransform`, the covariance function writes:

.. math::
  :label: TheoIntegral

    C_{i,j}^{stat}(\tau)  = \int_{\mathbb{R}}\exp\left\{  2i\pi f \tau \right\} S_{i,j}(f)\, df

where :math:`C_{i,j}^{stat}` is the element :math:`(i,j)` of the
matrix :math:`C^{stat}(\tau)` and :math:`S_{i,j}(f)` the one of
:math:`S(f)`. The integral :eq:`TheoIntegral` is approximated by its
evaluation on the finite domain :math:`\Omega \in \Rset`:

.. math::
  :label: finiteApproximation

    C_{i,j}^{stat}(\tau)  = \int_{\Omega}\exp\left\{  2i\pi f \tau \right\} S_{i,j}(f)\, df

Let us consider the partition of the domain as follows:

-  :math:`\Omega =[-\Omega_c, \Omega_c]` is subdivided into :math:`M`
   segments :math:`\Omega` = :math:`\cup_{k=1}^{M} \mathcal{M}_k` with
   :math:`\mathcal{M}_k=[f_k - \frac{\Delta f}{2}, f_k + \frac{\Delta f}{2}]`

-  :math:`\Delta f` be the frequency step,
   :math:`\Delta f = \frac{2 \Omega_c}{M}`

-  :math:`f_k` be the frequencies on which the spectral density is
   computed,
   :math:`f_k = -\Omega_c + \left(k - \frac{1}{2} \right) \Delta f = \left( 2 k - 1 - M \right) \frac{\Delta f}{2}`
   with :math:`k=1,2,\hdots,M`

The equation :eq:`finiteApproximation` can be rewritten as:

.. math:: C_{i,j}^{stat}(\tau) = \sum_{k=1}^{M}\int_{\mathcal{M}_k}\exp\left\{  2i\pi f \tau \right\} S_{i,j}(f)\, df

We focus on the integral on each subdomain :math:`\mathcal{M}_k`. Using
numerical approximation, we have:

.. math:: \int_{\mathcal{M}_k}\exp\left\{  2i\pi f \tau \right\} S_{i,j}(f)\, df \approx \Delta f S_{i,j}(f_k) \exp\left\{  2i\pi f_k \tau \right\}

:math:`\tau` must match with frequency values with
respect to the Shannon criteria. Thus the temporal domain of estimation
is the following:

-  :math:`\Delta t` is the time step,
   :math:`\Delta t = \frac{1}{2 \Omega_c}` such as
   :math:`\Delta f \Delta t = \frac{1}{M}`

-  :math:`\tilde{\mathcal{T}}` =\ :math:`[-T, T]` is subdivided into
   :math:`M` segments :math:`\tilde{{\mathcal{T}}}` =
   :math:`\cup_{m=1}^{M} \mathcal{T}_m` with
   :math:`\mathcal{T}_m=[t_m - \frac{\Delta t}{2}, t_m + \frac{\Delta t}{2}]`

-  :math:`t_m` be the time values on which the covariance is estimated,
   :math:`t_m = -\frac{M}{2 \Omega_c} + \left(m - \frac{1}{2} \right) \Delta t = \left(2 m - 1 - M \right) \frac{\Delta t}{2}`

The estimate of the covariance value at time value :math:`\tau_{m}`
depends on the quantities of form:

.. math::
  :label: integral_subdomain_spectral

    \int_{\mathcal{M}_k}\exp\left\{  2i\pi f \tau_{m} \right\} S_{i,j}(f)\, df \approx \Delta f S_{i,j}(f_k) \exp\left\{  2i\pi f_k \tau_{m} \right\}

We develop the expression of :math:`f_k` and :math:`\tau_{m}` and we
get:

.. math::

   \left\{
     \begin{array}{l}
       2m - 1 - M = 2 (m-1) - (M-1) \\
       2k - 1 - M = 2 (k-1) - (M-1)
     \end{array}
     \right.

 Thus,

.. math:: (2m - 1 - M) (2k - 1 - M) = 4 (m-1)(k-1) - (M-1)(2m -1 -M) - 2 (k-1)(M-1)

and:

.. math:: (2m - 1 - M) (2k - 1 - M)\frac{\Delta t}{2}\frac{\Delta f}{2} = \frac{(m-1)(k-1)}{M} - \frac{(M-1)(2m -1 -M)}{4M} - \frac{(k-1)(M-1)}{2M}

We denote :

.. math::

   \left\{
     \begin{array}{l}
       \delta(m) = \exp\left\{-i \frac{\pi}{2M} (M-1)(2m -1 -M) \right\}\\
       \phi_k = \exp\left\{-i \frac{\pi}{M} (k-1)(M-1) \right\} S_{i,j}(f_k)
     \end{array}
     \right.

Finally, we get the following expression for integral in
:eq:`integral_subdomain_spectral`:

.. math:: \int_{\mathcal{M}_k}\exp\left\{  2i\pi f \tau_{m} \right\} S_{i,j}(f)\, df \approx \Delta f \exp\left\{2i \frac{\pi}{M} (m-1)(k-1)  \right\} \delta(m) \phi_k

It follows that:

.. math::
  :label: expression_covariance_tm

    C_{i,j}^{stat}(\tau_m)  = \Delta f \delta(m) \sum_{k=1}^{M} \phi_k * \exp\left\{2i \frac{\pi}{M} (m-1)(k-1)  \right\}

.. topic:: API:

    - See :class:`~openturns.StationaryCovarianceModelFactory`
    - See :class:`~openturns.WelchFactory`
    - See :class:`~openturns.Hanning`

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/estimate_stationary_covariance_model`
