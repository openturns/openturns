.. _kernel_smoothing:

Kernel smoothing
----------------

Kernel smoothing is a non parametric estimation method of the probability density function of a distribution.

In dimension 1, the kernel smoothed probability density function :math:`\hat{p}` has the following expression,
where *K* is the univariate kernel, *n* the sample size and :math:`(X_1, \cdots, X_n) \in \Rset^n`
the univariate random sample with :math:`\forall i, \, \, X_i \in \Rset`:

.. math::
  :label: kernelSmooth

    \hat{p}(x) = \displaystyle \frac{1}{nh}\sum_{i=1}^{n} K\left(\frac{x-X_i}{h}\right)

The kernel *K* is a function satisfying :math:`\int K(x)\, dx=1`.
Usually *K* is chosen to be a unimodal probability density function that is symmetric about 0.
The parameter *h* is called the *bandwidth*.


In dimension :math:`d>1`, the kernel may be defined as a product kernel :math:`K_d`,
as follows where :math:`\vect{x} = (x_1, \cdots, x_d)\in \Rset^d`:

.. math::

    K_d(\vect{x}) = \displaystyle \prod_{j=1}^{d} K(x_j)

which leads to the kernel smoothed probability density function in dimension *d*,
where :math:`(\vect{X}_1, \cdots, \vect{X}_n)` is the d-variate random sample
which components are denoted :math:`\vect{X}_i = (X_{i1}, \dots, X_{id})`:

.. math::

    \hat{p}(\vect{x}) = \displaystyle \frac{1}{N \prod_{j=1}^{d}h_j} \sum_{i=1}^{N} K_d\left(\frac{x_1 - X_{i1} }{h_1}, \dots, \frac{x_d - X_{id}}{h_d}\right)

Let's note that the bandwidth is the vector :math:`\vect{h} = (h_1, \cdots, h_d)`.

The quality of the approximation may be controlled by the AMISE (Asymptotic Mean Integrated Square error) criteria defined as:

.. math::

  \left\{
  \begin{array}{lcl}
    AMISE(\hat{p}) & = & \mbox{two first terms in the series expansion with respect to $n$ in } MISE(\hat{p}) \\
    MISE(\hat{p}) & = & \mathbb{E}_\vect{X}\left[||\hat{p} - p||^2_{L_2}\right]   =  \int \, MSE(\hat{p}, \vect{x}) \, d\vect{x}  \\
    MSE(\hat{p}, \vect{x})&  =  & \left[ \mathbb{E}_\vect{X}\left[\hat{p}(\vect{x})\right] - p(\vect{x})\right]^2 + {\rm Var}_\vect{X}\left[\hat{p}(\vect{x})\right]
  \end{array}
  \right.

The quality of the estimation essentially depends on the value of the bandwidth *h*.
The bandwidth that minimizes the AMISE criteria  has the expression (given in dimension 1):

.. math::
  :label: AMISE1

  h_{AMISE}(K) = \displaystyle \left[ \frac{R(K)}{\mu_2(K)^2R(p^{(2)})}\right]^{\frac{1}{5}}n^{-\frac{1}{5}}

where :math:`R(K) = \int K(\vect{x})^2\, d\vect{x}` and :math:`\mu_2(K) = \int \vect{x}^2K(\vect{x})\, d\vect{x} = \sigma_K^2`.

If we note that :math:`R(p^{(r)}) = (-1)^r\Phi_{2r}` with :math:`\Phi_r = \int p^{(r)}p(x)\, dx = \mathbb{E}_\vect{X}\left[p^{(r)}\right]`,
then relation writes:

.. math::
  :label: AMISE

  h_{AMISE}(K) = \displaystyle \left[ \frac{R(K)}{\mu_2(K)^2\Phi_4}\right]^{\frac{1}{5}}n^{-\frac{1}{5}}

Several rules exist to  evaluate the optimal bandwidth :math:`h_{AMISE}(K)`:
all efforts are concentrated on the evaluation of the term :math:`\Phi_4`. We give here the most usual rules:

- the *Silverman rule* in dimension 1,
- the plug-in bandwidth selection
- the *Scott rule* in dimension d.




Silverman rule (dimension 1)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In the case where the density *p* is normal with standard deviation :math:`\sigma`,
then the term :math:`\Phi_4` can be exactly evaluated.
In that particular case,  the optimal bandwidth of relation :eq:`AMISE` with respect to the AMISE criteria writes as follows:

.. math::
  :label: pNormal

    h^{p = normal}_{AMISE}(K) = \displaystyle \left[ \frac{8\sqrt{\pi} R(K)}{3\mu_2(K)^2}\right]^{\frac{1}{5}}\sigma n^{-\frac{1}{5}}

An estimator of :math:`h^{p= normal}_{AMISE}(K)` is obtained by replacing :math:`\sigma` by its estimator :math:`\hat{\sigma}^n`,
evaluated from the sample :math:`(X_1, \dots, X_n)`:

.. math::
  :label: Estimpnormal

    \hat{h}^{p = normal}_{AMISE}(K) = \displaystyle \left[ \frac{8\sqrt{\pi} R(K)}{3\mu_2(K)^2}\right]^{\frac{1}{5}}\hat{\sigma}^n n^{-\frac{1}{5}}

The Silverman rule consists in considering :math:`\hat{h}^{p = normal}_{AMISE}(K)` of relation :eq:`Estimpnormal` even if the density *p* is not normal:

.. math::
  :label: Silverman

    h^{Silver}(K) = \displaystyle \left[ \frac{8\sqrt{\pi} R(K)}{3\mu_2(K)^2}\right]^{\frac{1}{5}}\hat{\sigma}^n n^{-\frac{1}{5}}

Relation :eq:`Silverman` is empirical and gives good results when the density is not *far* from a normal one.





Plug-in bandwidth selection method (dimension 1)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Relation :eq:`AMISE` requires the evaluation of the quantity :math:`\Phi_4`.
As a general rule, we use the estimator :math:`\hat{\Phi}_r` of :math:`\Phi_r` defined by:

.. math::
  :label: EstimPhir

    \hat{\Phi}_r = \displaystyle \frac{1}{n}\sum_{i=1}^{n} \hat{p}^{(r)}(X_i)

Deriving relation :eq:`kernelSmooth` leads to:

.. math::
  :label: kernelSmoothDerivative

    \hat{p}^{(r)}(x) = \displaystyle \frac{1}{nh^{r+1}}\sum_{i=1}^{n} K^{(r)}\left(\frac{x-X_i}{h}\right)

and then the estimator :math:`\hat{\Phi}_r(h)` is defined as:

.. math::
  :label: EstimPhirFin

    \hat{\Phi}_r(h) = \displaystyle \frac{1}{n^2h^{r+1}}\sum_{i=1}^{n}\sum_{j=1}^{n} K^{(r)}\left(\frac{X_i-X_j}{h}\right)

We note that :math:`\hat{\Phi}_r(h)` depends of the parameter *h* which can be
taken in order to minimize the AMSE (Asymptotic Mean Square Error) criteria
evaluated between :math:`\Phi_r` and :math:`\hat{\Phi}_r(h)`.
The optimal parameter *h* is:

.. math::
  :label: optimHamse

    h^{(r)}_{AMSE} = \displaystyle \left(\frac{-2K^{(r)}(0)}{\mu_2(K)\Phi_{r+2}}\right)^{\frac{1}{r+3}}n^{-\frac{1}{r+3}}

Given that preliminary results, the solve-the-equation plug-in method  proceeds as follows:

- Relation :eq:`AMISE` defines :math:`h_{AMISE}(K)` as a function of :math:`\Phi_4` we denote here as:

  .. math::
    :label: rel1

      h_{AMISE}(K) = t(\Phi_4)

- The term :math:`\Phi_4` is approximated by its estimator defined in
  :eq:`EstimPhirFin` evaluated with its optimal parameter :math:`h^{(4)}_{AMSE}`
  defined in :eq:`optimHamse`:

  .. math::
    :label: h4

      h^{(4)}_{AMSE} = \displaystyle \left(\frac{-2K^{(4)}(0)}{\mu_2(K)\Phi_{6}}\right)^{\frac{1}{7}}n^{-\frac{1}{7}}

  which leads to a relation of type:

  .. math::
    :label: rel2

      \Phi_4 \simeq  \hat{\Phi}_4(h^{(4)}_{AMSE})

- Relations :eq:`AMISE` and :eq:`h4` lead to the new one:

  .. math::
    :label: h4hAmise

      h^{(4)}_{AMSE} = \displaystyle \left( \frac{-2K^{(4)}(0)\mu_2(K)\Phi_4}{R(K)\Phi_{6}}\right) ^{\frac{1}{7}}h_{AMISE}(K)^{\frac{5}{7}}

  which rewrites:

  .. math::
    :label: rel3

      h^{(4)}_{AMSE} = l(h_{AMISE}(K))

- Relation :eq:`h4hAmise` depends on both terms :math:`\Phi_4` and
  :math:`\Phi_6` which are evaluated with their estimators defined in :eq:`EstimPhirFin`
  respectively with their AMSE optimal parameters :math:`g_1` and :math:`g_2`
  (see relation :eq:`optimHamse`). It leads to the expressions:

  .. math::
    :label: g12

      \left\{
      \begin{array}{lcl}
        g_1 & = & \displaystyle \left(\frac{-2K^{(4)}(0)}{\mu_2(K)\Phi_{6}}\right)^{\frac{1}{7}}n^{-\frac{1}{7}}\\
        g_2 & = & \displaystyle \left(\frac{-2K^{(6)}(0)}{\mu_2(K)\Phi_{8}}\right)^{\frac{1}{7}}n^{-\frac{1}{9}}
      \end{array}
      \right.

- In order to evaluate :math:`\Phi_6` and :math:`\Phi_8`,
  we suppose that the density *p* is normal with a variance :math:`\sigma^2`
  which is approximated by the empirical variance of the sample, which leads to:

  .. math::
    :label: Phi68

    \left\{
    \begin{array}{lcl}
      \hat{\Phi}_6 & = & \displaystyle \frac{-15}{16\sqrt{\pi}}\hat{\sigma}^{-7}\\
      \hat{\Phi}_8 & = & \displaystyle \frac{105^{\strut}}{32\sqrt{\pi}}\hat{\sigma}^{-9}
    \end{array}
    \right.

Then, to summarize, thanks to relations :eq:`rel1`, :eq:`rel2`, :eq:`rel3`, :eq:`g12` and :eq:`Phi68`, the optimal bandwidth is solution of the equation:

.. math::
  :label: equhAmise

    \boldsymbol{h_{AMISE}(K) = t \circ \hat{\Phi}_4 \circ l (h_{AMISE}(K))}



Scott rule (dimension d)
~~~~~~~~~~~~~~~~~~~~~~~~

The Scott rule is a simplification of the Silverman rule generalized to the
dimension *d* which is optimal when the density *p* is normal with independent components.
In all the other cases, it gives an empirical rule that gives good result when the density *p* is not *far* from the normal one.
For examples, the Scott bandwidth may appear too large when *p* presents several maximum.

The Silverman rule given in dimension 1 in relation :eq:`Silverman` can be generalized in dimension *d* as follows:
if we suppose  that the density *p* is normal with independent components,
in dimension *d* and that we use the normal kernel :math:`N(0,1)` to estimate it,
then the optimal bandwidth vector :math:`\vect{h}` with respect to the AMISE criteria writes as follows:

.. math::
  :label: SilvermanNormalKernel

    \vect{h}^{Silver}(N) = \left(\left(\frac{4}{d+2}\right)^{1/(d+4)}\hat{\sigma}_i^n n^{-1/(d+4)}\right)_i

where :math:`\hat{\sigma}_i^n` is the standard deviation of the *i*-th component of the sample
:math:`(\vect{X}_1, \cdots, \vect{X}_n)`, and :math:`\sigma_K` the standard deviation of the 1D kernel *K*.



The Scott proposition is  a simplification of the Silverman rule, based on the fact that the coefficient
:math:`\left(\frac{4}{d+2}\right)^{1/(d+4)}` remains in :math:`[0.924, 1.059]` when the dimension *d* varies.
Thus, Scott fixed it to *1*:

.. math::
  :label: coefficientScott

    \left(\frac{4}{d+2}\right)^{1/(d+4)} \simeq 1

which leads to the simplified expression:

.. math::
  :label: SilvermanNormalKernelSimplif

    \vect{h}^{Silver}(N) \simeq \left( \hat{\sigma}_i^n n^{-1/(d+4)}\right)_i


Furthermore, in the general case, we have from relation (\ref{AMISE1}) :

.. math::
  :label: ChangeBandwidth

    \frac{h_{AMISE}(K_1)}{h_{AMISE}(K_2)}=\frac{\sigma_{K_2}}{\sigma_{K_1}}\left[\frac{\sigma_{K_1}R(K_1)}{\sigma_{K_2}R(K_2)}\right]^{1/5}

Considering that :math:`\sigma_{K}R(K) \simeq 1` whatever the kernel *K*, relation :eq:`ChangeBandwidth` simplifies in:

.. math::
  :label: SimplifiedChangeBandwidth

    h_{AMISE}(K_1) \simeq h_{AMISE}(K_2)\frac{\sigma_{K_2}}{\sigma_{K_1}}

If we consider the normal kernel :math:`N(0,1)` for :math:`K_2`, then relation :eq:`SimplifiedChangeBandwidth` writes in a more general notation:

.. math::
  :label: SimplifiedChangeBandwidthNormal

    h_{AMISE}(K) \simeq h_{AMISE}(N)\frac{1}{\sigma_{K}}

If :math:`h_{AMISE}(N)` is evaluated with the Silverman rule, :eq:`SimplifiedChangeBandwidthNormal` rewrites:

.. math::
  :label: SimplifiedChangeBandwidthSilvNormal

    h^{Silver}(K) \simeq h^{Silver}(N)\frac{1}{\sigma_{K}}

At last, from relation :eq:`SilvermanNormalKernelSimplif` and :eq:`SimplifiedChangeBandwidthSilvNormal`
applied in each direction *i*, we deduce the Scott rule:

.. math::
  :label: ScottRule

    \boldsymbol{\vect{h}^{Scott} = \left(\frac{\hat{\sigma}_i^n}{\sigma_K}n^{-1/(d+4)}\right)_i}



Boundary treatment
~~~~~~~~~~~~~~~~~~

In dimension 1, the boundary effects may be taken into account:
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

.. topic:: API:

    - See the :class:`~openturns.KernelSmoothing` factory

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/estimate_non_parametric_distribution`

.. topic:: References:

     - *Kernel smoothing*, M.P. Wand and M.C. Jones, Chapman & Hall/CRC edition, ISNB 0-412-55270-1.
     - *Multivariate Density Estimation, practice and Visualisation, Theory*, David W. Scott, Wiley edition.
