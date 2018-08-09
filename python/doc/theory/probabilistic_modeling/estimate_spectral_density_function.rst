.. _estimate_spectral_density_function:

Estimation of a spectral density function
==========================================

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
| This objective of this use case is to estimate the spectral density
  function :math:`S` from data, which can be a sample of time series or
  one time series.

Depending on the available data, we proceed differently:

-  if the data correspond to several independent realizations of the
   process, a *statistical
   estimate* is performed using statistical average of a
   realization-based estimator;

-  if the data correspond to one realization of the process at different
   time stamps (stored in a *TimeSeries* object), the process being
   observed during a long period of time, an *ergodic estimate* is
   performed using a time average of an ergodic-based estimator.

| The estimation of the spectral density function from data may use some
  parametric or non parametric methods.
| The *Welch* method is a *non parametric* estimation technique, known
  to be performant. We detail it in the case where the available data on
  the process is a time series which values are
  :math:`(\vect{x}_0, \dots,\vect{x}_{N-1})` associated to the time grid
  :math:`(t_0, \dots, t_{N-1})` which is a discretization of the domain
  :math:`[0,T]`.
| We assume that the process has a spectral density :math:`S` defined on
  :math:`| f | \leq \frac{T}{2}`.
| The method is based on the segmentation of the time series into
  :math:`K` segments of length :math:`L`, possibly overlapping (size of
  overlap :math:`R`).
| Let :math:`\vect{X}_{1}(j), \ j = 0, 1,...,L-1` be the first such
  segment. Then:

  .. math:: \vect{X}_{1}(j) = \vect{X}(j) , \ j = 0, 1,...,L-1

  Applying the same decomposition,

  .. math:: \vect{X}_{2}(j) = \vect{X}(j + (L - R)) , \ j = 0, 1,...,L-1

  and finally:

  .. math:: \vect{X}_{K}(j) = \vect{X}(j + (K-1)(L-R)) , \ j = 0, 1,...,L-1

The objective is to get a statistical estimator from these :math:`K`
segments. We define the *periodogram* associated with the segment
:math:`\vect{X}_k` by:

.. math::

   \begin{aligned}
     \vect{X}_{k}(f_p,T)&=&\Delta t\sum_{n=0}^{L-1}\vect{x}(n\Delta t)\exp\left[\frac{-j2\pi pn}{N}\right], \quad p=0,\dots, L-1\\
     \hat{G}_{\vect{x}}(f_p,T)&=&\frac{2}{T}\vect{X}_{k}(f_p,T)\,{\vect{X}_{k}(f_p,T)^*}^t,\quad p=0,\dots,L/2-1\end{aligned}

with :math:`\Delta t=\frac{T}{N}` and
:math:`f_p=\frac{p}{T}=\frac{p}{N}\frac{1}{\Delta t}`.

| It has been proven that the *periodogram* has bad statistical
  properties. Indeed, two quantities summarize the properties of an
  estimator: its *bias* and its *variance*. The bias is the expected
  error one makes on the average using only a finite number of time
  series of finite length, whereas the covariance is the expected
  fluctuations of the estimator around its mean value. For the
  periodogram, we have:

-  Bias\ :math:`=\mathbb{E}[\hat{G}_{\vect{x}}(f_p, T)-G_{\vect{X}}(f_p)]=(\frac{1}{T}W_B(f_p, T)-\delta_0)*G_{\vect{X}}(f_p)`
   where :math:`W_B(f_p, T) = \left(\frac{\sin\pi fT}{\pi fT}\right)^2`
   is the squared module of the Fourier transform of the function
   :math:`w_B(t, T)` (*Barlett window*) defined by:

   .. math:: w_B(t, T) = \mathbf{1}_{[0,T]}(t)

   This estimator is *biased* but this bias vanishes when
   :math:`T\rightarrow\infty` as
   :math:`\lim_{T\rightarrow\infty} \frac{1}{T}W_B(f_p, T)=\delta_0`.

-  Covariance\ :math:`=\frac{1}{T}W_B(f_p, T)*G_{\vect{X}}(f_p)\rightarrow G_{\vect{X}}(f_p)`
   as :math:`T\rightarrow\infty`, which means that the fluctuations of
   the periodogram are of the same order of magnitude as the quantity to
   be estimated and thus the estimator is not convergent.

The periodogram’s lack of convergence may be easily fixed if we consider
the *averaged periodogram* over :math:`K` independent time series or
segments:

.. math:: \hat{G}_{\vect{x}}(f_p,T)=\frac{2}{KT}\sum_{k=0}^{K-1}\vect{X}^{(k)}(f_p,T)\vect{X}^{(k)}(f_p,T)^t

The averaging process has no effect on the significant bias of the
periodogram.

The use of a *tapering window* :math:`w(t, T)` may significantly reduce
it. The time series :math:`\vect{x}(t)` is replaced by a tapered time
series :math:`w(t, T)\vect{x}(t)` in the computation of
:math:`\vect{X}(f_p,T)`. One gets:

.. math:: \mathbb{E}[\hat{G}_{\vect{x}}(f_p, T)-G_{\vect{X}}(f_p)=(\frac{1}{T}W(f_p, T)-\delta_0)*G_{\vect{X}}(f_p)

where :math:`W(f_p, T)` is the square module of the Fourier transform
of :math:`w(t, T)` at the frequency :math:`f_p`. A judicious choice of
tapering function such as the *Hanning window* :math:`w_H` can
dramatically reduce the bias of the estimate:

.. math::
  :label: HamEff

    w_H(t, T) = \sqrt{\frac{8}{3}}\left(1-\cos^2\left(\frac{\pi t}{T}\right)\right)\mathbf{1}_{[0,T]}(t)

The library builds an estimation of the spectrum on a *TimeSeries* by
fixing the number of segments, the *overlap* size parameter and a
*FilteringWindows*. The available ones are:

-  The *Hamming* window

   .. math:: w(t, T) = \sqrt{\frac{1}{K}}\left(0.54-0.46\cos^2\left(\frac{2 \pi t}{T}\right)\right)\mathbf{1}_{[0,T]}(t)

   with :math:`K` = :math:`\sqrt{0.54^2 + \frac{1}{2} 0.46^2}`

-  The *Hanning* window described in :eq:`HamEff` which is supposed to be
   the most useful.

.. topic:: API:

    - See :class:`~openturns.Hanning`
    - See :class:`~openturns.Hamming`
    - See :class:`~openturns.WelchFactory`

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/estimate_spectral_density_function`

