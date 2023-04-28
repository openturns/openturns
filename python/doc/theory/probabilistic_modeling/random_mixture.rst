.. _random_mixture:

Random Mixture: affine combination of independent univariate distributions
--------------------------------------------------------------------------

Let :math:`\vect{Y}` be the random vector defined as the
affine transform of :math:`n` independent univariate random variable.
More precisely, consider:

.. math::
  :label: RandomMixtureFormula

   \vect{Y}=\vect{y}_0+\mat{M}\,\vect{X}

where :math:`\vect{y}_0\in\mathbb{R}^d` is a deterministic vector with
:math:`d\in\{1,2,3\}`, :math:`\mat{M}\in\mathcal{M}_{d,n}(\mathbb{R})` is a
deterministic matrix and :math:`\left(X_k\right)_{ 1 \leq k \leq n}` are
independent random variables.
In this case, it is possible to directly evaluate the distribution of
:math:`\vect{Y}` and then to ask :math:`\vect{Y}` any request compatible
with a distribution: moments, probability and cumulative density
functions, quantiles (in dimension 1 only)...
In this document, we present a method using the Poisson summation
formula to evaluate the distribution of :math:`\vect{Y}`.

Evaluation of the probability density function of the Random Mixture
--------------------------------------------------------------------

Since, by hypothesis, the univariate random variables :math:`X_i` are independent, the
characteristic function of :math:`\vect{Y}`, denoted :math:`\phi_Y`, is
easily defined from the characteristic function of :math:`X_k` denoted
:math:`\phi_{X_k}` as follows:

.. math::
  :label: CharactFuncY

   \phi_Y(u_1,\hdots,u_d)
   = \prod_{j=1}^d e^{iu_j{y_0}_j} \prod_{k=1}^n\phi_{X_k}\left(\left(M^t u\right)_k\right),

for any :math:`\vect{u}\in\mathbb{R}^d`.
Once :math:`\phi_Y` is evaluated, it is possible to evaluate the
probability density function of :math:`Y`, denoted :math:`p_Y` :
several techniques are possible, as the inversion of the Fourier
transformation, but this method is not easy to implement.
We can alternatively use the Poisson summation formula:

.. math::
  :label: PoissonSum

   & \sum_{j_1\in\mathbb{Z}}\hdots\sum_{j_d\in\mathbb{Z}} p_Y\left(y_1+\frac{2\pi j_1}{h_1},\hdots,y_d+\frac{2\pi j_d}{h_d}\right) \\
   & = \left(\prod_{j=1}^d \frac{h_j}{2 \pi} \right) \sum_{k_1\in\mathbb{Z}}\hdots\sum_{k_d\in\mathbb{Z}}\phi\left(k_1h_1,\hdots,k_dh_d\right)e^{-\imath \left(\sum_{m=1}^{d}k_m h_m y_m\right)}

where :math:`\imath` is the complex imaginary number, i.e. :math:`\imath^2 = -1`.
If :math:`h_1,\hdots,h_d` are close to zero, then:

.. math:: \frac{2k\pi}{h_j} \approx +\infty

and:

.. math:: p_Y\left(\hdots,\frac{2k\pi}{h_j},\hdots\right) \approx 0

because of the decreasing properties of :math:`p_Y`. Thus the nested sums of the left
term of :eq:`PoissonSum` are reduced to the central term
:math:`j_1=\hdots=j_d = 0`: the left term is approximately equal to
:math:`p_Y(y)`.
Furthermore, the right term of :eq:`PoissonSum` is a series which
converges very fast: few terms of the series are enough to get
machine-precision accuracy. Let us note that the factors
:math:`\phi_Y(k_1 h_1,\hdots,k_d,h_d)`, which are expensive to
evaluate, do not depend on :math:`y` and are evaluated once only.

It is also possible to greatly improve the performance of the
algorithm by noticing that the equation is linear between :math:`p_Y` and
:math:`\phi_Y`. We denote by :math:`q_Y` and :math:`\psi_Y` respectively
the density and the characteristic function of the multivariate normal
distribution with the same mean :math:`\vect{\mu}` and same covariance
matrix :math:`\vect{C}` as the random mixture. By applying this
multivariate normal distribution to the equation, we obtain by
subtraction:

.. math::
    :label: algoPoisson

     p_Y\left(y\right)
     & = \sum_{j\in\mathbb{Z}^d} q_Y\left(y_1+\frac{2\pi j_1}{h_1},\cdots,y_d+\frac{2\pi j_d}{h_d}\right) \\
     & \quad + \frac{H}{2^d\pi^d}\sum_{|k_1|\leq N}\cdots\sum_{|k_d|\leq N} \delta_Y\left(k_1h_1,\cdots,k_dh_d\right)e^{-\imath \left(\sum_{m=1}^{d}k_m h_m y_m\right)}

where :math:`H = h_1\times\cdots\times h_d`,
:math:`j=(j_1,\cdots,j_d)` and :math:`\delta_Y:=\phi_Y - \psi_Y`.
In the case where :math:`n \gg 1`, using the limit central theorem,
the law of :math:`\vect{Y}` tends to the normal distribution density
:math:`q`, which will drastically reduce :math:`N`. The sum on
:math:`q` will become the most CPU-intensive part, because in the
general case we will have to keep more terms than the central one in
this sum, since the parameters :math:`h_1, \dots  h_d` were
calibrated with respect to :math:`p` and not :math:`q`.

The parameters :math:`h_1, \dots  h_d` are calibrated using the
following formula:

.. math::  h_\ell = \frac{2\pi}{(\beta+4\alpha)\sigma_\ell}

where :math:`\sigma_\ell=\sqrt{\Cov{\vect{Y}}_{\ell,\ell}}` and
:math:`\alpha`, :math:`\beta` are respectively the number of standard
deviations covered by the marginal distribution (:math:`\alpha=5` by
default) and :math:`\beta` the number of marginal deviations beyond
which the density is negligible (:math:`\beta=8.5` by default).
The parameter :math:`N` is dynamically calibrated: we start with
:math:`N=8` then we double :math:`N` value until the total contribution
of the additional terms is negligible.

Evaluation of the moments of the Random Mixture
-----------------------------------------------

The relation :eq:`RandomMixtureFormula` enables to evaluate all the
moments of the random mixture, if mathematically defined. For example,
we have:

.. math::

      \Expect{\vect{Y}} & = \vect{y_0} + \mat{M}\Expect{\vect{X}} \\
      \Cov{\vect{Y}} & = \mat{M}\,\Cov{\vect{X}}\mat{M}^t

Computation on a regular grid
-----------------------------

We want to compute the density function on a regular grid and
to get an approximation quickly.
The regular grid is:

.. math::


   \:y_{r,m}=\mu_r+b\left(\frac{2m+1}{M} - 1\right)\sigma_r

for all :math:`r\in\{1,\hdots,d\}` and :math:`m\in\{0,\hdots,M-1\}`.
Denoting :math:`p_{m_1,\hdots,m_d}=p_{\vect{Y}}(y_{1,m_1},\hdots,y_{d,m_d})`:

.. math::

   p_{m_1,\hdots,m_d}= Q_{m_1,\hdots,m_d}+S_{m_1,\hdots,m_d}

for which the term :math:`S_{m_1,\hdots,m_d}` is the most CPU
consuming. This term rewrites:

.. math::

   S_{m_1,\hdots,m_d}=&\frac{H}{2^d\pi^d}\sum_{k_1=-N}^{N}\hdots\sum_{k_d=-N}^{N}\delta\left(k_1h_1,\hdots,k_dh_d\right)
   E_{m_1,\hdots,m_d}(k_1,\hdots,k_d)

with:

.. math::

   \delta\left(k_1h_1,\hdots,k_dh_d\right) & = (\phi-\psi)\left(k_1h_1,\hdots,k_dh_d\right)\\
   E_{m_1,\hdots,m_d}(k_1,\hdots,k_d) & = e^{-i\sum_{j=1}^d k_jh_j\left(\mu_j+b\left(\frac{2m_j+1}{M}-1\right)\sigma_j\right)}

The aim is to rewrite the previous expression as a :math:`d`- discrete
Fourier transform, in order to apply Fast Fourier Transform (*FFT*) for
its evaluation.
We set :math:`M=N` and
:math:`\forall j \in\{1,\hdots,d\},\: h_j=\frac{\pi}{b\sigma_j}` and
:math:`\tau_j=\frac{\mu_j}{b\sigma_j}`. For convenience, we introduce
the functions:

.. math::

    f_j(k) = e^{-i\pi (k+1)\left(\tau_j-1+\frac{1}{N}\right)}

We use :math:`k+1` instead of :math:`k` in this function to simplify
expressions below.
We obtain:

.. math::

   & E_{m_1,\hdots,m_d}(k_1,\hdots,k_d) \\
   & = e^{-i\sum_{j=1}^{d} k_jh_jb\sigma_j\left(\frac{\mu_j}{b\sigma_j}+\frac{2m_j}{N}+\frac{1}{N}-1\right)}\\
   & = e^{-2i\pi\left(\frac{\sum_{j=1}^{d}k_j m_j}{N}\right)}e^{-i\pi\sum_{j=1}^{d} k_j\left(\tau_j-1+\frac{1}{N}\right)} \\
   & = e^{-2i\pi\left(\frac{\sum_{j=1}^{d}k_j m_j}{N}\right)} f_1(k_1-1) \times \hdots \times f_d(k_d-1)

For performance reasons, we want to use the discrete Fourier transform
with the following convention in dimension 1:

.. math:: A_m = \sum_{k=0}^{N-1} a_k e^{-2i\pi\frac{km}{N}}

which extension to dimensions 2 and 3 are respectively:

.. math::

    A_{m,n} & = \sum_{k=0}^{N-1}\sum_{l=0}^{N-1} a_{k,l} e^{-2i\pi\frac{km}{N}} e^{-2i\pi\frac{ln}{N}}\\
    A_{m,n,p} & = \sum_{k=0}^{N-1}\sum_{l=0}^{N-1}\sum_{s=0}^{N-1} a_{k,l,s} e^{-2i\pi\frac{km}{N}} e^{-2i\pi\frac{ln}{N}} e^{-2i\pi\frac{sp}{N}}

We decompose sums of on the interval :math:`[-N,N]` into three parts:

.. math::
 :label: decomposition-sum

     & \sum_{k_j=-N}^{N}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}(k_1,\hdots,k_d) \\
     & = \sum_{k_j=-N}^{-1} \delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}(k_1,\hdots,k_d) \\
     & \quad + \delta\left(k_1h_1,\hdots,0,\hdots,k_dh_d\right) E_{m_1,\hdots,0,\hdots,m_d}(k_1,\hdots,0,\hdots,k_d) \\
     & \quad+ \sum_{k_j=1}^{N}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}(k_1,\hdots,k_d)

If we compute :math:`E` for dimension :math:`d-1`, then the
middle term in this sum is trivial.

To compute the last sum, we apply a change of variable :math:`k_j' = k_j-1`:

.. math::

     & \sum_{k_j=1}^{N}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}(k_1,\hdots,k_d) \\
     & = \sum_{k_j=0}^{N-1}\delta\left(k_1h_1,\hdots,(k_j+1)h_j,\hdots,k_dh_d\right) \times\\
     & \quad E_{m_1,\hdots,m_d}(k_1,\hdots,k_j+1,\hdots,k_d)

This implies:

.. math::

     & E_{m_1,\hdots,m_d}(k_1, \hdots, k_j+1, \hdots, k_d) \\
     &= e^{-2i\pi\left(\frac{\sum_{l = 1}^{d}k_l m_l}{N} +\frac{m_j}{N}\right)}
         f_1(k_1 - 1)\times \hdots \times f_j(k_j) \times \hdots \times f_d(k_d - 1) \\
     &= e^{-2i\pi\left(\frac{m_j}{N}\right)}
         e^{-2i\pi\left(\frac{\sum_{l = 1}^{d}k_l m_l}{N}\right)}
         f_1(k_1 - 1)\times \hdots \times f_j(k_j) \times \hdots \times f_d(k_d - 1)

Thus:

.. math::

     & \sum_{k_j=1}^{N}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}(k_1,\hdots,k_d) \\
     & = e^{-2i\pi\left(\frac{m_j}{N}\right)} \sum_{k_j=0}^{N-1}\delta\left(k_1h_1,\hdots,(k_j+1)h_j,\hdots,k_dh_d\right) \times \\
     & \quad e^{-2i\pi\left(\frac{\sum_{l=1}^{d}k_l m_l}{N}\right)}
         f_1(k_1-1)\times \hdots \times f_j(k_j)\times \hdots \times f_d(k_d-1)

To compute the first sum of equation, we apply a change of variable
:math:`k_j'=N+k_j`:

.. math::

     & \sum_{k_j=-N}^{-1}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d}(k_1,\hdots,k_d) \\
     &=  \sum_{k_j=0}^{N-1}\delta\left(k_1h_1,\hdots,(k_j-N)h_j,\hdots,k_dh_d\right) \times \\
       & \quad  E_{m_1,\hdots,m_d}(k_1,\hdots,k_j-N,\hdots,k_d)

This implies:

.. math::

     & E_{m_1,\hdots,m_d}(k_1,\hdots,k_j-N,\hdots,k_d) \\
     &= e^{-2i\pi\left(\frac{\sum_{l=1}^{d}k_l m_l}{N} -m_j\right)}
         f_1(k_1-1)\times \hdots \times f_j(k_j-1-N)\times \hdots \times f_d(k_d-1) \\
     & = e^{-2i\pi\left(\frac{\sum_{l=1}^{d}k_l m_l}{N}\right)}
         f_1(k_1-1)\times \hdots \times \overline{f}_j(N-1-k_j)\times \hdots \times f_d(k_d-1)

Thus:

.. math::

     & \sum_{k_j=-N}^{-1}\delta\left(k_1h_1,\hdots,k_dh_d\right) E_{m_1,\hdots,m_d} (k_1,\hdots,k_d) \\
     & = \sum_{k_j=0}^{N-1}\delta\left(k_1h_1,\hdots,(k_j-N)h_j,\hdots,k_dh_d\right) \times \\
     & \quad e^{-2i\pi\left(\frac{\sum_{l=1}^{d}k_l m_l}{N}\right)}
         f_1(k_1-1)\times \hdots \times \overline{f}_j(N-1-k_j)\times \hdots \times f_d(k_d-1)

To summarize:

#. In order to compute sum from :math:`k_1=1` to :math:`N`, we multiply
   by :math:`e^{-2i\pi\left(\frac{m_1}{N}\right)}` and consider
   :math:`\delta((k_1+1)h,\hdots)f_1(k_1)`

#. In order to compute sum from :math:`k_1=-N` to :math:`-1`, we
   consider :math:`\delta((k_1-N)h,\hdots)\overline{f}_1(N-1-k_1)`


.. topic:: API:

    - See :class:`~openturns.RandomMixture`

.. topic:: Examples:

    - See :doc:`/auto_probabilistic_modeling/distributions/plot_create_random_mixture`

.. topic:: References:

    -  [abate1992]_
