Random Mixture: affine combination of independent univariate distributions
--------------------------------------------------------------------------

A multivariate random variable :math:`\vect{Y}` may be defined as an
affine transform of :math:`n` independent univariate random variable, as
follows:

.. math::
  :label: RandomMixtureFormula

   \displaystyle \vect{Y}=\vect{y}_0+\mat{M}\,\vect{X}

where :math:`\vect{y}_0\in\mathbb{R}^d` is a deterministic vector with
:math:`d\in\{1,2,3\}`, :math:`\mat{M}\in\mathcal{M}_{d,n}(\mathbb{R})` a
deterministic matrix and :math:`(X_k)_{ 1 \leq k \leq n}` are some
independent univariate distributions.

In such a case, it is possible to evaluate directly the distribution of
:math:`\vect{Y}` and then to ask :math:`\vect{Y}` any request compatible
with a distribution: moments, probability and cumulative density
functions, quantiles (in dimension 1 only) ...

**Evaluation of the probability density function of the Random Mixture**

As the univariate random variables :math:`X_i` are independent, the
characteristic function of :math:`\vect{Y}`, denoted :math:`\phi_Y`, is
easily defined from the characteristic function of :math:`X_k` denoted
:math:`\phi_{X_k}` as follows :

.. math::

   \displaystyle \phi_Y(u_1,\hdots,u_d)=\prod_{j=1}^de^{iu_j{y_0}_j}\prod_{k=1}^n\phi_{X_k}((M^tu)_k), \mbox{  for } \vect{u}\in\mathbb{R}^d
       \label{CharactFuncY}

| Once :math:`\phi_Y` evaluated, it is possible to evaluate the
  probability density function of :math:`Y`, denoted :math:`p_Y` :
  several techniques are possible, as the inversion of the Fourier
  transformation. This technique is not easy to implement.
| Another technique is used, based on the Poisson sum
  formulation, defined as follows:

  .. math::
    :label: PoissonSum

     \displaystyle \sum_{j_1\in\mathbb{Z}}\hdots\sum_{j_d\in\mathbb{Z}} p_Y\left(y_1+\frac{2\pi j_1}{h_1},\hdots,y_d+\frac{2\pi j_d}{h_d}\right)=
         \prod_{j=1}^d \frac{h_j}{2*\pi}\sum_{k_1\in\mathbb{Z}}\hdots\sum_{k_d\in\mathbb{Z}}\phi\left(k_1h_1,\hdots,k_dh_d\right)e^{-\imath(\sum_{m=1}^{d}k_m h_m y_m)}

| By fixing :math:`h_1,\hdots,h_d` small enough,
  :math:`\frac{2k\pi}{h_j} \approx +\infty` and
  :math:`p_Y(\hdots,\frac{2k\pi}{h_j},\hdots) \approx 0` because of the
  decreasing properties of :math:`p_Y`. Thus the nested sums of the left
  term of :eq:`PoissonSum` are reduced to the central term
  :math:`j_1=\hdots=j_d = 0`: the left term is approximatively equal to
  :math:`p_Y(y)`.
| Furthermore, the right term of :eq:`PoissonSum` is a series which
  converges very fast: only few terms of the series are enough to get
  machine-precision accuracy. Let us note that the factors
  :math:`\phi_Y(k_1 h_1,\hdots,k_d,h_d)`, which are expensive to
  evaluate, do not depend on :math:`y` and are evaluated once only.

| It is also possible to greatly improve the performance of the
  algorithm by noticing that equation  is linear between :math:`p_Y` and
  :math:`\phi_Y`. We denote :math:`q_Y` and :math:`\psi_Y` respectively
  the density and the characteristic function of the multivariate normal
  distribution with the same mean :math:`\vect{\mu}` and same covariance
  matrix :math:`\vect{C}` as the random mixture. By applying this
  multivariate normal distribution to the equation , we obtain by
  subtraction:

  .. math::

     \displaystyle  p_Y\left(y\right) = \sum_{j\in\mathbb{Z}^d} q_Y\left(y_1+\frac{2\pi j_1}{h_1},\cdots,y_d+\frac{2\pi j_d}{h_d}\right)+
       \frac{H}{2^d\pi^d}\sum_{|k_1|\leq N}\cdots\sum_{|k_d|\leq N} \delta_Y\left(k_1h_1,\cdots,k_dh_d\right)e^{-\imath(\sum_{m=1}^{d}k_m h_m y_m)}
       \label{algoPoisson}

where :math:`H = h_1\times\cdots\times h_d`,
:math:`j=(j_1,\cdots,j_d)`, :math:`\delta_Y:=\phi_Y - \psi_Y`

| In the case where :math:`n \gg 1`, using the limit central theorem,
  the law of :math:`\vect{Y}` tends to the normal distribution density
  :math:`q`, which will drastically reduce :math:`N`. The sum on
  :math:`q` will become the most CPU-intensive part, because in the
  general case we will have to keep more terms than the central one in
  this sum, since the parameters :math:`h_1, \dots  h_d` were
  calibrated with respect to :math:`p` and not :math:`q`.

The parameters :math:`h_1, \dots  h_d` are calibrated using the
following formula:

.. math::

   \begin{aligned}
       h_\ell = \frac{2\pi}{(\beta+4\alpha)\sigma_\ell}
     \end{aligned}

where :math:`\sigma_\ell=\sqrt{\Cov{\vect{Y}}_{\ell,\ell}}` and
:math:`\alpha`, :math:`\beta` are respectively the number of standard
deviations covered by the marginal distribution (:math:`\alpha=5` by
default) and :math:`\beta` the number of marginal deviations beyond
which the density is negligible (:math:`\beta=8.5` by default).

The :math:`N` parameter is dynamically calibrated: we start with
:math:`N=8` then we double :math:`N` value until the total contribution
of the additional terms is negligible.

**Evaluation of the moments of the Random Mixture**

The relation :eq:`RandomMixtureFormula` enables to evaluate all the
moments of the random mixture, if mathematically defined. For example,
we have:

.. math::

    \left\{
    \begin{array}{lcl}
      \Expect{\vect{Y}} & = & \vect{y_0} + \mat{M}\Expect{\vect{X}} \\
      \Cov{\vect{Y}} & = & \mat{M}\,\Cov{\vect{X}}\mat{M}^t
    \end{array}\right\}

**Computation on a regular grid**

The interest is to compute the density function on a regular grid.
Purposes are to get an approximation quickly. The regular grid is of
form:

.. math::

   \begin{aligned}
       \forall r\in\{1,\hdots,d\},\forall m\in\{0,\hdots,M-1\},\:y_{r,m}=\mu_r+b\left(\frac{2m+1}{M} - 1\right)\sigma_r
     \end{aligned}

By denoting :math:`p_{m_1,\hdots,m_d}=p_{\vect{Y}}(y_{1,m_1},\hdots,y_{d,m_d})`:

.. math::

   \begin{aligned}
       p_{m_1,\hdots,m_d}= Q_{m_1,\hdots,m_d}+S_{m_1,\hdots,m_d}
     \end{aligned}

for which the term :math:`S_{m_1,\hdots,m_d}` is the most CPU
consuming. This term rewrites:

.. math::

   \begin{aligned}
     S_{m_1,\hdots,m_d}=&\frac{H}{2^d\pi^d}\sum_{k_1=-N}^{N}\hdots\sum_{k_d=-N}^{N}\delta\left(k_1h_1,\hdots,k_dh_d\right)
     E_{m_1,\hdots,m_d}(k_1,\hdots,k_d) \label{Eq:S}
     \end{aligned}

with:

.. math::

   \begin{aligned}
       \delta\left(k_1h_1,\hdots,k_dh_d\right)&=(\phi-\psi)\left(k_1h_1,\hdots,k_dh_d\right)\\
       E_{m_1,\hdots,m_d}(k_1,\hdots,k_d)&=e^{-i\sum_{j=1}^d k_jh_j\left(\mu_j+b\left(\frac{2m_j+1}{M}-1\right)\sigma_j\right)}
     \end{aligned}

The aim is to rewrite the previous expression as a :math:`d`- discrete
Fourier transform, in order to apply Fast Fourier Transform (*FFT*) for
its evaluation.

We set :math:`M=N` and
:math:`\forall j \in\{1,\hdots,d\},\: h_j=\frac{\pi}{b\sigma_j}` and
:math:`\tau_j=\frac{\mu_j}{b\sigma_j}`. For convenience, we introduce
the functions:

.. math:: f_j(k) = e^{-i\pi (k+1)\left(\tau_j-1+\frac{1}{N}\right)}

We use :math:`k+1` instead of :math:`k` in this function to simplify
expressions below.

We obtain:

.. math::

   \begin{aligned}
     E_{m_1,\hdots,m_d}(k_1,\hdots,k_d)&=e^{-i\sum_{j=1}^{d} k_jh_jb\sigma_j\left(\frac{\mu_j}{b\sigma_j}+\frac{2m_j}{N}+\frac{1}{N}-1\right)}\notag\\
       &=e^{-2i\pi\left(\frac{\sum_{j=1}^{d}k_j m_j}{N}\right)}e^{-i\pi\sum_{j=1}^{d} k_j\left(\tau_j-1+\frac{1}{N}\right)} \notag\\
       &=e^{-2i\pi\left(\frac{\sum_{j=1}^{d}k_j m_j}{N}\right)} f_1(k_1-1) \times\hdots\times f_d(k_d-1) \label{Eq:E}
     \end{aligned}

For performance reasons, we want to use the discrete Fourier transform
with the following convention in dimension 1:

.. math:: A_m = \sum_{k=0}^{N-1} a_k e^{-2i\pi\frac{km}{N}}

which extension to dimensions 2 and 3 are respectively:

.. math:: A_{m,n} = \sum_{k=0}^{N-1}\sum_{l=0}^{N-1} a_{k,l} e^{-2i\pi\frac{km}{N}} e^{-2i\pi\frac{ln}{N}}\\

.. math:: A_{m,n,p} = \sum_{k=0}^{N-1}\sum_{l=0}^{N-1}\sum_{s=0}^{N-1} a_{k,l,s} e^{-2i\pi\frac{km}{N}} e^{-2i\pi\frac{ln}{N}} e^{-2i\pi\frac{sp}{N}}

We decompose sums of  on the interval :math:`[-N,N]` into three parts:

.. math::

   \begin{aligned}
     \sum_{k_j=-N}^{N}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}(k_1,\hdots,k_d)
       = & \sum_{k_j=-N}^{-1} \delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}(k_1,\hdots,k_d) \notag\\
       & + \delta\left(k_1h_1,\hdots,0,\hdots,k_dh_d\right) E_{m_1,\hdots,0,\hdots,m_d}(k_1,\hdots,0,\hdots,k_d) \notag\\
       & + \sum_{k_j=1}^{N}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}(k_1,\hdots,k_d) \label{Eq:decomposition-sum}
     \end{aligned}

If we already computed :math:`E` for dimension :math:`d-1`, then the
middle term in this sum is trivial.

To compute the last sum of equation, we apply a change of variable
:math:`k_j'=k_j-1`:

.. math::

   \begin{aligned}
     \sum_{k_j=1}^{N}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}(k_1,\hdots,k_d)
     = & \sum_{k_j=0}^{N-1}\delta\left(k_1h_1,\hdots,(k_j+1)h_j,\hdots,k_dh_d\right) \times\notag\\
       & \hspace*{3cm} E_{m_1,\hdots,m_d}(k_1,\hdots,k_j+1,\hdots,k_d)
     \end{aligned}

Equation gives:

.. math::

   \begin{aligned}
     E_{m_1,\hdots,m_d}(k_1,\hdots,k_j+1,\hdots,k_d) 
     &= 
         e^{-2i\pi\left(\frac{\sum_{l=1}^{d}k_l m_l}{N} +\frac{m_j}{N}\right)}
         f_1(k_1-1)\times\hdots\times f_j(k_j)\times\hdots\times f_d(k_d-1)\notag\\
     &= 
         e^{-2i\pi\left(\frac{m_j}{N}\right)}
         e^{-2i\pi\left(\frac{\sum_{l=1}^{d}k_l m_l}{N}\right)}
         f_1(k_1-1)\times\hdots\times f_j(k_j)\times\hdots\times f_d(k_d-1)
     \end{aligned}

Thus

.. math::

   \begin{aligned}
     \sum_{k_j=1}^{N}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}&(k_1,\hdots,k_d)
       = e^{-2i\pi\left(\frac{m_j}{N}\right)} \sum_{k_j=0}^{N-1}\delta\left(k_1h_1,\hdots,(k_j+1)h_j,\hdots,k_dh_d\right) \times\notag\\
       & e^{-2i\pi\left(\frac{\sum_{l=1}^{d}k_l m_l}{N}\right)}
         f_1(k_1-1)\times\hdots\times f_j(k_j)\times\hdots\times f_d(k_d-1) \label{Eq:j-sigma+}
     \end{aligned}

To compute the first sum of equation, we apply a change of variable
:math:`k_j'=N+k_j`:

.. math::

   \begin{aligned}
     \sum_{k_j=-N}^{-1}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}(k_1,\hdots,k_d)
     = & \sum_{k_j=0}^{N-1}\delta\left(k_1h_1,\hdots,(k_j-N)h_j,\hdots,k_dh_d\right) \times\notag\\
       & \hspace*{3cm} E_{m_1,\hdots,m_d}(k_1,\hdots,k_j-N,\hdots,k_d)
     \end{aligned}

Equation  gives:

.. math::

   \begin{aligned}
     E_{m_1,\hdots,m_d}(k_1,\hdots,k_j-N,\hdots,k_d) 
     &= 
         e^{-2i\pi\left(\frac{\sum_{l=1}^{d}k_l m_l}{N} -m_j\right)}
         f_1(k_1-1)\times\hdots\times f_j(k_j-1-N)\times\hdots\times f_d(k_d-1) \notag\\
     &= 
         e^{-2i\pi\left(\frac{\sum_{l=1}^{d}k_l m_l}{N}\right)}
         f_1(k_1-1)\times\hdots\times \overline{f}_j(N-1-k_j)\times\hdots\times f_d(k_d-1) 
     \end{aligned}

Thus:

.. math::

   \begin{aligned}
     \sum_{k_j=-N}^{-1}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}&(k_1,\hdots,k_d)
       = \sum_{k_j=0}^{N-1}\delta\left(k_1h_1,\hdots,(k_j-N)h_j,\hdots,k_dh_d\right) \times\notag\\
       & e^{-2i\pi\left(\frac{\sum_{l=1}^{d}k_l m_l}{N}\right)}
         f_1(k_1-1)\times\hdots\times \overline{f}_j(N-1-k_j)\times\hdots\times f_d(k_d-1) \label{Eq:j-sigma-}
     \end{aligned}

To summarize:

#. In order to compute sum from :math:`k_1=1` to :math:`N`, we multiply
   by :math:`e^{-2i\pi\left(\frac{m_1}{N}\right)}` and consider
   :math:`\delta((k_1+1)h,\hdots)f_1(k_1)`

#. In order to compute sum from :math:`k_1=-N` to :math:`-1`, we
   consider :math:`\delta((k_1-N)h,\hdots)\overline{f}_1(N-1-k_1)`


.. topic:: API:

    - See :class:`~openturns.RandomMixture`

.. topic:: Examples:

    - See :doc:`/examples/probabilistic_modeling/random_mixture_distribution`
    - See :doc:`/examples/probabilistic_modeling/random_mixture_distribution_discrete`

.. topic:: References:

    - Abate, J. and Whitt, W. (1992). *The Fourier-series method for inverting transforms of probability distributions*. Queueing Systems 10, 5--88., 1992, formula 5.5.
