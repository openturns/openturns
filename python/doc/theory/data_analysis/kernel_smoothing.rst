.. _kernel_smoothing:

Kernel smoothing
----------------

Kernel smoothing is a non parametric estimation method of the probability density function of a distribution.

Introduction
~~~~~~~~~~~~

Let :math:`X` be a random variable with probability density function :math:`p`.
Given a sample of independent observations :math:`x_1, ..., x_n` of :math:`X`
and any point :math:`x \in \Rset`, the kernel smoothing estimator provides
an approximation :math:`\widehat{p}(x)` of :math:`p(x)`.

In dimension 1, the kernel smoothed probability density function :math:`\widehat{p}` has the following expression,
where *K* is the univariate kernel, *n* the sample size and :math:`(X_1, \cdots, X_n) \in \Rset^n`
the univariate random sample with :math:`\forall i, \, \, X_i \in \Rset` ([wand1994]_ eq. 2.2 page 12):

.. math::
  :label: kernelSmooth

    \widehat{p}(x) = \frac{1}{nh}\sum_{i=1}^{n} K\left(\frac{x-X_i}{h}\right)

The kernel *K* is a function satisfying:

.. math::

    \int K(x)\, dx=1.

The parameter *h* is called the *bandwidth*.

Multivariate kernels
~~~~~~~~~~~~~~~~~~~~

In dimension :math:`d>1`, the kernel may be defined as a product kernel :math:`K_d`,
as follows where :math:`\vect{x} = (x_1, \cdots, x_d)\in \Rset^d`
([wand1994]_ eq. 2.2 page 91):

.. math::

    K_d(\vect{x}) = \prod_{j=1}^{d} K(x_j).

The kernel smoothed probability density function in dimension *d* is:

.. math::

    \widehat{p}(\vect{x})
    = \frac{1}{n \prod_{j=1}^{d}h_j} \sum_{i=1}^{n} K_d\left(\frac{x_1 - (X_{i})_1 }{h_1}, \dots, \frac{x_d - (X_{i})_d}{h_d}\right)

where :math:`(\vect{X}_1, \cdots, \vect{X}_n)` is the d-variate random sample
which components are denoted :math:`\vect{X}_i = (X_{i1}, \dots, X_{id})`.

In the multivariate case, the bandwidth is the vector
:math:`\vect{h} = (h_1, \cdots, h_d)`.

Asymptotic error and asymptotically optimal bandwidth
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The quality of the approximation may be controlled by the AMISE (Asymptotic Mean Integrated Square error) criteria defined as follows.
By definition, the mean squared error (MSE) at point :math:`\vect{x}` is
(see [scott2015]_ eq. page 40, [wand1994]_ pages 14-21):

.. math::

    \operatorname{MSE}(\widehat{p}, \vect{x})
    = \mathbb{E}_\vect{X}\left[\left(\widehat{p}(\vect{x}) - p(\vect{x})\right)^2 \right].


It can be proved that the mean squared error is the sum of the
variance and the squared bias:

.. math::

    \operatorname{MSE}(\widehat{p}, \vect{x})
    = \operatorname{Var}_\vect{X}\left[\widehat{p}(\vect{x})\right]
    + \left(\operatorname{Bias}\left(\widehat{p}(\vect{x})\right)\right)^2

where the bias is:

.. math::
    \operatorname{Bias}\left(\widehat{p}(\vect{x})\right)
    = \mathbb{E}_\vect{X}\left[\widehat{p}(\vect{x})\right] - p(\vect{x}).

The MSE depends on the point where the density is evaluated.
The mean integrated squared error (MISE) is (see [scott2015]_ eq. page 41):

.. math::
    \operatorname{MISE}(\widehat{p})
    = \mathbb{E}_\vect{X}\left[||\widehat{p} - p||^2_{L_2}\right]   = \int \, \operatorname{MSE}(\widehat{p}, \vect{x}) \, d\vect{x}  \\

Finally, the asymptotic mean integrated squared error (AMISE),
denoted :math:`\operatorname{AMISE}(\widehat{p})` is defined as the two first terms
in the Taylor series expansion of :math:`\operatorname{MISE}(\widehat{p})` when :math:`n`
tends to infinity.

The quality of the estimation essentially depends on the value of the bandwidth *h*.
In dimension 1, the bandwidth that minimizes the AMISE criteria is
(see [wand1994]_ eq 2.13 page 22):

.. math::
  :label: AMISE1

  h_{\operatorname{AMISE}}(K)
  = \left( \frac{R(K)}{\mu_2(K)^2 R\left(p^{(2)}\right)}\right)^{\frac{1}{5}}n^{-\frac{1}{5}}

where the rugosity of the kernel is:

.. math::
    R(K) = \int K(\vect{x})^2\, d\vect{x}

and the second raw moment of the kernel is:

.. math::
    \mu_2(K) = \int \vect{x}^2K(\vect{x})\, d\vect{x} = \sigma_K^2.

In the equation :eq:`AMISE1`, the expression :math:`R\left(p^{(2)}\right)` is the rugosity of
the second derivative of the density probability function :math:`p` that
we wish to approximate.
Since, by hypothesis, the true density :math:`p` is unknown, its
second derivative is also unknown.
Hence the equation :eq:`AMISE1` cannot be used directly to compute the bandwidth.

We have ([wand1994]_ page 67):

.. math::
    R\left(p^{(r)}\right) = (-1)^r\Psi_{2r}

where:

.. math::
    \Psi_r(p)
    = \int p^{(r)}p(x)\, dx = \mathbb{E}_\vect{X}\left[p^{(r)}\right].

Therefore:

.. math::
  :label: AMISE

  h_{\operatorname{AMISE}}(K)
  = \left( \frac{R(K)}{\mu_2(K)^2\Psi_4(p)}\right)^{\frac{1}{5}}n^{-\frac{1}{5}}

Several methods exist to  evaluate the optimal bandwidth :math:`h_{\operatorname{AMISE}}(K)` based on different approximations of :math:`\Psi_4`:

- Silverman's rule in dimension 1,
- the plug-in bandwidth selection,
- Scott's rule in dimension d.

Efficiency of a kernel
~~~~~~~~~~~~~~~~~~~~~~

Let :math:`K` be a kernel.
We may be interested if a particular kernel may be able to reduce the
estimation error.
The efficiency of a kernel is (see [scott2015]_ page 151):

.. math::
    \operatorname{eff}(k)
    = \frac{\sigma_k R(k)}{\sigma_{k_E} R(k_E)}

where :math:`k_E` is Epanechnikov's kernel.
The AMISE error is proportional to the efficiency (see [scott2015]_
eq. 6.25 page 151):

.. math::
    \operatorname{AMISE} \propto \operatorname{eff}(k)

The next table presents several kernels available in the
library and their associated variance, rugosity and efficiency.
We see that the best kernel is Epanechnikov's kernel.
We also see that there is not much difference between the different
kernels.
This is one of the reasons why the normal kernel is often used.

+-----------------+---------------------------------+----------------+--------------------------+-----------------------------------+
| Kernel          | :math:`\operatorname{Var}(k)`   | :math:`R(k)`   | :math:`\sigma_k R(k)`    | :math:`\operatorname{eff}(k)`     |
+=================+=================================+================+==========================+===================================+
| Epanechnikov    | 0.2000                          | 0.6000         | 0.2683                   | 100.00 \%                         |
+-----------------+---------------------------------+----------------+--------------------------+-----------------------------------+
| Biweight        | 0.1429                          | 0.7143         | 0.2700                   | 99.39 \%                          |
+-----------------+---------------------------------+----------------+--------------------------+-----------------------------------+
| Quartic         | 0.1429                          | 0.7143         | 0.2700                   | 99.39 \%                          |
+-----------------+---------------------------------+----------------+--------------------------+-----------------------------------+
| Triweight       | 0.1111                          | 0.8159         | 0.2720                   | 98.67 \%                          |
+-----------------+---------------------------------+----------------+--------------------------+-----------------------------------+
| Triangular      | 0.1667                          | 0.6667         | 0.2722                   | 98.59 \%                          |
+-----------------+---------------------------------+----------------+--------------------------+-----------------------------------+
| Normal          | 1.0000                          | 0.2821         | 0.2821                   | 95.12 \%                          |
+-----------------+---------------------------------+----------------+--------------------------+-----------------------------------+
| Uniform         | 0.3333                          | 0.5000         | 0.2887                   | 92.95 \%                          |
+-----------------+---------------------------------+----------------+--------------------------+-----------------------------------+
| Logistic        | 3.2899                          | 0.1667         | 0.3023                   | 88.76 \%                          |
+-----------------+---------------------------------+----------------+--------------------------+-----------------------------------+

**Table 1.** Efficiency of several order 2 kernels.

Silverman's rule (dimension 1)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this section, we consider a random variable i.e. :math:`d = 1`.
If the density *p* is normal with standard deviation :math:`\sigma`,
then the term :math:`\Psi_4` can be exactly evaluated.
By definition, the Silverman rule for the bandwidth is
the optimal bandwidth of the AMISE criteria when the true density *p* is normal
(see [silverman1986]_ page 45):

.. math::
  :label: Silverman

    h^{Silver}(K)
    := h^{p = normal}_{\operatorname{AMISE}}(K)
    = \left( \frac{8\sqrt{\pi} R(K)}{3\mu_2(K)^2}\right)^{\frac{1}{5}}
    \sigma n^{-\frac{1}{5}}.

The Silverman rule is based on the hypothesis that the true
density *p* is close to the normal density,
even if the density *p* is not necessarily normal.

The equation :eq:`Silverman` is accurate when
the density is not *far* from a normal one.
In the special case where we use the normal kernel, the Silverman rule
is (see [silverman1986]_ eq 3.28 page 45):

.. math::
  :label: SilvermanNormal

    h^{Silver}(\mathcal{N})
    = 1.06 \sigma n^{-\frac{1}{5}}.

Choice for the standard deviation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We estimate the standard deviation :math:`\sigma` by its sample
counterpart :math:`\hat{\sigma}`, evaluated from the sample
:math:`(x_1, \dots, x_n)`:

.. math::
  :label: Estimpnormal

    h^{Silver}(K)
    = \left( \frac{8\sqrt{\pi} R(K)}{3\mu_2(K)^2}\right)^{\frac{1}{5}}
    \hat{\sigma} n^{-\frac{1}{5}}

The estimator :math:`\hat{\sigma}` of the true standard deviation can
be estimated using the sample standard deviation based
on the sample :math:`(x_1, \dots, x_n)`.
This is:

.. math::
    \hat{\sigma}
    = \sqrt{\frac{1}{n - 1} \sum_{i = 1}^n (x_i - \bar{x})^2 }

where :math:`\bar{x}` is the sample mean:

.. math::
    \bar{x}
    = \frac{1}{n} \sum_{i = 1}^n x_i.

Another method is to use the standardized interquartile range
([wand1994]_ page 60):

.. math::
    \widehat{\sigma}_{\operatorname{IQR}}
    = \frac{\widehat{q}(3/4) - \widehat{q}(1/4)}{\Phi^{-1}(3/4) - \Phi^{-1}(1/4)}

where :math:`\Phi^{-1}` is the quantile function of the
standard normal distribution and
:math:`\widehat{q}(3/4)` and :math:`\widehat{q}(1/4)` are the sample
quartiles at levels 75% and 25% respectively.
The previous estimator is robust against outliers that might be
in the sample.

Plug-in bandwidth selection method (dimension 1)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The plug-in bandwidth selection method improves the estimation of the rugosity of the second
derivative of the density.
Instead of making the Gaussian assumption, the method uses a kernel smoothing method
in order to make an approximation of higher derivatives of the density.
This method is due to [sheather1991]_ who used ideas from [park1990]_.
The algorithm is presented in [wand1994]_, page 74 under the "Solve the equation rule" name.
The implementation uses ideas from [raykar2006]_, but the fast selection is not implemented.

The equation :eq:`AMISE` requires the evaluation of the quantity :math:`\Psi_4`.
We use the estimator :math:`\hat{\Psi}_r` of :math:`\Psi_r`, using a kernel
density estimator of the :math:`r`-th derivative of the
density.
The estimator is (see [wand1994]_ page 64):

.. math::
  :label: EstimPhir

    \hat{\Psi}_r(K)
    = \frac{1}{n}\sum_{i=1}^{n} \widehat{p}^{(r)}(X_i)

where :math:`\hat{\Psi}_r(K)` is the estimator based on the kernel
*K*.
Deriving equation :eq:`kernelSmooth` leads to:

.. math::
  :label: kernelSmoothDerivative

    \widehat{p}^{(r)}(x)
    = \frac{1}{nh^{r+1}}\sum_{i=1}^{n} K^{(r)}\left(\frac{x-X_i}{h}\right)

and then the estimator :math:`\hat{\Psi}_r(h)` is defined as:

.. math::
  :label: EstimPhirFin

    \hat{\Psi}_r(h)
    = \frac{1}{n^2h^{r+1}}\sum_{i=1}^{n}\sum_{j=1}^{n} K^{(r)}\left(\frac{X_i-X_j}{h}\right)

We note that :math:`\hat{\Psi}_r(h)` depends of the parameter *h* which can be
taken in order to minimize the Asymptotic Mean Square Error (AMSE) criteria
evaluated between :math:`\Psi_r` and :math:`\hat{\Psi}_r(h)`.
The optimal parameter *h* is:

.. math::
  :label: optimHamse

    h^{(r)}_{\operatorname{AMSE}}
    = \left(\frac{-2K^{(r)}(0)}{\mu_2(K) \Psi_{r+2}}\right)^{\frac{1}{r+3}}n^{-\frac{1}{r+3}}

The previous equation states that the bandwidth :math:`h^{(r)}` required
to compute :math:`\widehat{p}^{(r)}` depends on :math:`\Psi_{r+2}`.
But to compute :math:`\Psi_{r+2}`, we need :math:`h^{(r + 2)}`, etc.
The goal of the method is to break this infinite set of equations at some point
by providing a *pilot* bandwidth.
The :math:`\ell`-stage plug-in bandwidth method uses :math:`\ell` different
intermediate bandwidths before evaluating the final one.

In this document, we present the two stage solve-the-equation plug-in method.

- The equation :eq:`AMISE` defines :math:`h_{\operatorname{AMISE}}(K)` as a function of :math:`\Psi_4`.
  Let :math:`t` be the function defined by the equation:

  .. math::
    :label: rel1

      h_{\operatorname{AMSE}}(K) = t(\Psi_4).

- The term :math:`\Psi_4` is approximated by its estimator defined in
  :eq:`EstimPhirFin` evaluated with its optimal parameter :math:`h^{(4)}_{\operatorname{AMSE}}`
  defined in :eq:`optimHamse`:

  .. math::
    :label: h4

      h^{(4)}_{\operatorname{AMSE}}
      = \left(\frac{-2K^{(4)}(0)}{\mu_2(K)\Psi_{6}}\right)^{\frac{1}{7}}n^{-\frac{1}{7}}

  which leads to the approximation:

  .. math::
    :label: rel2

      \hat{\Psi}_4 \left(h^{(4)}_{\operatorname{AMSE}}\right) \approx  \Psi_4

- The equation :eq:`AMISE` and :eq:`h4` lead to:

  .. math::
    :label: h4hAmise

      h^{(4)}_{\operatorname{AMSE}}
      = \left( \frac{-2K^{(4)}(0)\mu_2(K)\Psi_4}{R(K)\Psi_{6}}\right) ^{\frac{1}{7}}h_{\operatorname{AMISE}}(K)^{\frac{5}{7}}.

  Let :math:`\ell` be the function defined by the equation:

  .. math::
    :label: rel3

      h^{(4)}_{\operatorname{AMSE}}
      = \ell(h_{\operatorname{AMISE}}(K)).

- The equation :eq:`h4hAmise` depends on both terms :math:`\Psi_4` and
  :math:`\Psi_6` which are evaluated with their estimators defined in :eq:`EstimPhirFin`
  respectively with their AMSE optimal parameters :math:`g_1` and :math:`g_2`
  (see eq. :eq:`optimHamse`). It leads to the expressions:

  .. math::
    :label: g12

    g_1 & = \left(\frac{-2K^{(4)}(0)}{\mu_2(K)\Psi_{6}}\right)^{\frac{1}{7}}n^{-\frac{1}{7}}\\
    g_2 & = \left(\frac{-2K^{(6)}(0)}{\mu_2(K)\Psi_{8}}\right)^{\frac{1}{7}}n^{-\frac{1}{9}}

- In order to evaluate :math:`\Psi_6` and :math:`\Psi_8`,
  we assume that the density *p* is normal with a variance :math:`\sigma^2`
  which is approximated by the empirical variance of the sample, which leads to:

  .. math::
    :label: Phi68

    \hat{\Psi}_6 & = \frac{-15}{16\sqrt{\pi}}\hat{\sigma}^{-7}\\
    \hat{\Psi}_8 & = \frac{105^{\strut}}{32\sqrt{\pi}}\hat{\sigma}^{-9}

Finally, thanks to the equations :eq:`rel1`, :eq:`rel2`, :eq:`rel3`, :eq:`g12` and :eq:`Phi68`,
the optimal bandwidth of the STE rule, :math:`h^{\operatorname{STE}}`, is solution of the equation:

.. math::
  :label: equhAmise

    h^{\operatorname{STE}}
    = t \circ \hat{\Psi}_4 \circ \ell (h^{\operatorname{STE}})

This equation does not necessarily have a close form expression and
an numerical method must be used in general.

A cut-off value can be used to define the function :math:`\widehat{\psi_r}` in the equation :eq:`EstimPhirFin`.
Let :math:`\phi` be the probability density function of the standard Gaussian distribution.
We have:

.. math::
    \phi(x) \rightarrow 0

when :math:`|x|\rightarrow +\infty`, with a fast decrease to zero.
Let :math:`t> 0` the cut-off value.
The evaluation is as follows:

.. math::
    \widetilde{\phi}(x)=
    \begin{cases}
    \phi(x) & \textrm{ if } |x| \leq t, \\
    0 & \textrm{ otherwise}.
    \end{cases}

Hence, only the most significant values in the evaluation of :math:`\hat{\psi_r}`
are taken into account, which improves the speed while slightly decreasing
the accuracy.

Rescaling a bandwidth
~~~~~~~~~~~~~~~~~~~~~

In this section, we show that, if the optimal bandwidth is known
for the normal kernel, then it can be computed for any kernel
*K* using a rescaling equation.

Let :math:`K_1` and :math:`K_2` be two kernels.
The equation :eq:`AMISE1` implies:

.. math::
  :label: ChangeBandwidth

    \frac{h_{\operatorname{AMISE}}(K_1)}{h_{\operatorname{AMISE}}(K_2)}=\frac{\sigma_{K_2}}{\sigma_{K_1}}
    \left(\frac{\sigma_{K_1}R(K_1)}{\sigma_{K_2}R(K_2)}\right)^{1/5}.

Scott (see [scott2015]_ table 6.2 page 152) notices that:

.. math::
    \frac{\sigma_{K_1}R(K_1)}{\sigma_{K_2}R(K_2)} \in [1, 1.86]

for many pairs of common kernels.
Hence the equation :eq:`ChangeBandwidth` implies the *equivalent
kernel rescaling equation* (see [scott2015]_ eq. 6.30 page 154):

.. math::
  :label: SimplifiedChangeBandwidth

    h_{\operatorname{AMISE}}(K_1) \approx h_{\operatorname{AMISE}}(K_2)\frac{\sigma_{K_2}}{\sigma_{K_1}}

Consider for example the normal kernel :math:`K_2 = \mathcal{N}(0,1)`.
Since :math:`\sigma_{K_1} = \sigma_{\mathcal{N}(0,1)} = 1`,
then equation :eq:`SimplifiedChangeBandwidth` implies:

.. math::
  :label: SimplifiedChangeBandwidthNormal

    h_{\operatorname{AMISE}}(K) \approx h_{\operatorname{AMISE}}(\mathcal{N})\frac{1}{\sigma_{K}}

We will use the previous equation in the derivation of the
*mixed* rule presented in the next section.
The previous equation applied to the Silverman rule implies:

.. math::
  :label: SimplifiedChangeBandwidthSilvNormal

    h^{Silver}(K) \approx h^{Silver}(\mathcal{N})\frac{1}{\sigma_{K}}

A mixed rule for a large sample
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the sample size :math:`n` is large, the *solve-the-equation* (STE)
rule cannot be applied because of its CPU cost.
In this case, we use a method which combines the STE rule
and the Silverman rule, which is less costly.
Moreover, we combine these rules using two different kernels, namely
the kernel *K* given by the user and the normal kernel :math:`\mathcal{N}(0, 1)`.
Finally, we combine two different samples, that is to say the
large sample size :math:`n` and a smaller sample size for which the
STE rule can be evaluated.

The equation :eq:`AMISE` implies that:

.. math::
    \frac{h_{\operatorname{AMISE}}(K)}{h^{Silver}(K)}
    = \left(\frac{\Psi_4(\mathcal{N})}{\Psi_4(p)}\right)^{1/5}

where *K* is a given kernel and :math:`h_{\operatorname{AMISE}}(K)` is the
optimal AMISE bandwidth for the kernel *K*.
We notice that the previous ratio is independent from the sample
size :math:`n`.
Let :math:`n_t \ll n` be a small sample size.
Hence, the ratio is the same if we consider the sample size :math:`n`
or the sample size :math:`n_t`.
We apply this equation to the normal kernel, approximate the
AMISE optimal bandwidth by the STE rule and use the sample
sizes :math:`n` and :math:`n_t`.
We get:

.. math::
    \frac{h^{n, STE}(\mathcal{N})}{h^{n, Silver}(\mathcal{N})}
    \approx \frac{h^{n_t, STE}(\mathcal{N})}{h^{n_t, Silver}(\mathcal{N})}

which implies:

.. math::
    h^{n, STE}(\mathcal{N})
    \approx \frac{h^{n_t, STE}(\mathcal{N})}{h^{n_t, Silver}(\mathcal{N})}
    h^{n, Silver}(\mathcal{N})

The equation :eq:`SimplifiedChangeBandwidthNormal` leads to the
bandwidth of the STE rule for the kernel *K*:

.. math::
    h^{n, STE}(K)
    \approx h^{n, STE}(\mathcal{N}) \frac{1}{\sigma_{K}}.

We substitute the expression of :math:`h^{n, STE}` in the
previous equation and get the *mixed* rule:

.. math::
    :label: MixedBandwidthRule

    h^{n, STE}(K)
    \approx \frac{h^{n_t, STE}(\mathcal{N})}{h^{n_t, Silver}(\mathcal{N})}
    h^{n, Silver}(\mathcal{N}) \frac{1}{\sigma_{K}}.

Scott rule (dimension d)
~~~~~~~~~~~~~~~~~~~~~~~~

In this section, we consider the general case where the
random vector has dimension :math:`d`.
The Scott rule is a simplification of the Silverman rule generalized to the
dimension *d* which is optimal when the density *p* is normal with independent components.
In all the other cases, it gives an empirical rule that gives good result when the density *p* is not *far* from the normal one.
For examples, the Scott bandwidth may appear too large when *p* presents several maximum.

The Silverman rule given in dimension 1 in equation :eq:`Silverman` can be generalized in dimension *d* as follows.
We make the assumption that the density *p* is normal with independent components,
in dimension *d* and that we use the normal kernel :math:`\mathcal{N}(0,1)`
to estimate it.
Therefore the optimal bandwidth vector :math:`\vect{h}` with respect to the
AMISE criteria is given by the *normal reference rule* (see [scott2015]_ eq.
6.43 page 164):

.. math::
  :label: SilvermanNormalKernel

    \vect{h}^{Silver}(\mathcal{N}) = \left(\left(\frac{4}{d+2}\right)^{1 / (d + 4)}\hat{\sigma}_i^n n^{-1 / (d + 4)}\right)_i

where :math:`\hat{\sigma}_i^n` is the standard deviation of the *i*-th component of the sample
:math:`(\vect{X}_1, \cdots, \vect{X}_n)`, and :math:`\sigma_K` the standard deviation of the 1D kernel *K*.

Scott' method is  a simplification of Silverman's rule, based on the fact that the coefficient
:math:`\left(\frac{4}{d+2}\right)^{1 / (d + 4)}` remains in :math:`[0.924, 1.059]` when the dimension *d* varies (see [scott2015]_ page 164).
Thus, Scott fixed it to *1*:

.. math::
  :label: coefficientScott

    \left(\frac{4}{d+2}\right)^{1 / (d + 4)} \approx 1.

This leads to Scott's rule (see [scott2015]_ eq. 6.44 page 164):

.. math::
  :label: SilvermanNormalKernelSimplif

    \vect{h}^{Silver}(\mathcal{N}) \approx \left( \hat{\sigma}_i^n n^{-1 / (d + 4)}\right)_i

Finally, the equations :eq:`SilvermanNormalKernelSimplif` and :eq:`SimplifiedChangeBandwidthSilvNormal`
applied in each direction *i* imply:

.. math::
  :label: ScottRule

    \vect{h}^{Scott}
    = \left(\frac{\hat{\sigma}_i^n}{\sigma_K}n^{-1 / (d + 4)}\right)_i

for :math:`i = 1, ..., d`.

Boundary treatment
~~~~~~~~~~~~~~~~~~

In this section, we consider a random variable i.e. :math:`d = 1`.
Assume that the domain of definition of the density is bounded.
Then one of the problems of kernel smoothing is that it may
produce a non zero density estimate even in the regions where
we know it is zero.
This is known as the *boundary bias problem* (see [silverman1986]_ page 29).
The reason is that a subpart of the kernel windows does not contain
any observation ([wand1994]_ page 127).
In this case, for some observation :math:`x_i` near the boundary,
the density may be underestimated if the kernel sets a positive weight
outside the domain ([chacon2018]_ page 73).

There are several methods to solve this problem.
One of the methods is to apply a transformation to the data
(see [chacon2018]_ page 73).
Another method is to use boundary kernels (see [chacon2018]_ page 76,
[scott2015]_ page 157).

In dimension 1, the boundary effects may be taken into account using
a *reflection* or *mirroring* method (see [silverman1982]_ page 31).
the boundaries are automatically detected from the sample
(with the *min* and *max* functions) and the kernel smoothed PDF
is corrected in the boundary areas to remain within the boundaries,
according to the mirroring technique:

- the Scott bandwidth is evaluated from the sample: *h*

- two sub-samples are extracted from the initial sample,
  containing all the points within the range :math:`[min, min + h[` and  :math:`]max-h, max]`,

- both sub-samples are transformed into their symmetric samples with respect their respective boundary:
  its results two samples within the range :math:`]min-h, min]` and :math:`[max, max+h[`,

- a kernel smoothed PDF is built from the new sample composed with
  the initial one and the two new ones, with the previous bandwidth *h*,

- this last kernel smoothed PDF is truncated within the initial range :math:`[min, max]` (conditional PDF).

Conclusion
~~~~~~~~~~
The next table presents a summary of histogram, kernel smoothing and
parametric methods.
It appears that the kernel density estimator has an AMISE error which is
quite close to the parametric rate.

+------------------+-----------------------------------------+
| Method           | Optimal :math:`\operatorname{AMISE}`    |
+==================+=========================================+
| Histogram        | :math:`\propto n^{-\frac{2}{3}}`        |
+------------------+-----------------------------------------+
| Kernel smoothing | :math:`\propto n^{-\frac{4}{5}}`        |
+------------------+-----------------------------------------+
| Parametric       | :math:`\propto n^{-1}`                  |
+------------------+-----------------------------------------+

**Table 2.** The AMISE error depending on the method to estimate the density,
from the least to the most accurate.

The next table compare the different estimators of the
bandwidth that we have presented so far.
The best method is the STE rule, but this can be
costly to evaluate if the sample is large.
In this case the *mixed* rule can be used.
If this rule is still too large, then the Silverman rule can be
used and might be satisfactory if the true density *p*
is not too far away from the normal distribution (i.e.
unimodal and symmetric).
Otherwise, the Silverman rule may produce a too large bandwidth,
leading to oversmoothing.

+--------------------------+----------------------+---------------+----------------------------+
| Method                   | Assumption           | Cost          | Accuracy                   |
+==========================+======================+===============+============================+
| Silverman                | Normal assumption    | Low           | If *p* not far from normal |
+--------------------------+----------------------+---------------+----------------------------+
| Mixed                    | Normal assumption    | Moderate      | Intermediate               |
+--------------------------+----------------------+---------------+----------------------------+
| Solve-the-equation (STE) | Normal assumption    | High          | Accurate                   |
+--------------------------+----------------------+---------------+----------------------------+

**Table 3.** Different estimators of the bandwidth from the least to the
most accurate.


.. topic:: API:

    - See the :class:`~openturns.KernelSmoothing` factory

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/distribution_fitting/plot_estimate_non_parametric_distribution`

.. topic:: References:

     - [silverman1986]_
     - [wand1994]_
     - [scott2015]_
     - [sheather1991]_
     - [park1990]_
     - [raykar2006]_
     - [silverman1982]_
