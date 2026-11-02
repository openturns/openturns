.. _christoffel_sampling:

Christoffel distribution and sampling
=====================================

This page describes the Christoffel distribution associated with an
approximation space, the determinantal point processes (DPP) built on the
Christoffel kernel, and the sampling strategies of the
:class:`~openturns.experimental.ChristoffelSubsampleExperiment`. Throughout,
:math:`n` denotes the number of design points and :math:`m` the dimension of
the approximation space, as in the API. This is the opposite of the convention
of [cohendolbeault2020]_, where :math:`m` is the number of points and
:math:`n` the space dimension.

.. _christoffel_function:

Approximation space and Christoffel function
--------------------------------------------

Let :math:`\cD \subset \Rset^\inputDim` be a domain and :math:`\mu` a
probability measure on :math:`\cD`, with density :math:`p_\mu` with respect
to the Lebesgue measure. Let :math:`V_m \subset L^2(\cD, \mu)` be an
:math:`m`-dimensional approximation space, with an orthonormal basis
:math:`(\varphi_1, \dots, \varphi_m)`:

.. math::

    \scalarproduct{\varphi_i}{\varphi_j}_{L^2(\cD,\mu)}
    = \int_\cD \varphi_i(\vect{x}) \varphi_j(\vect{x}) \di{\mu(\vect{x})}
    = \delta_{ij},
    \qquad 1 \leq i, j \leq m.

The reproducing kernel of :math:`V_m` and the Christoffel function are

.. math::
    :label: christoffel_kernel

    K_m(\vect{x}, \vect{y}) = \sum_{j=1}^m \varphi_j(\vect{x}) \varphi_j(\vect{y}),
    \qquad
    k_m(\vect{x}) = K_m(\vect{x}, \vect{x}) = \sum_{j=1}^m \varphi_j(\vect{x})^2.

For any :math:`v \in V_m`, the reproducing property
:math:`v(\vect{x}) = \scalarproduct{v}{K_m(\vect{x}, \cdot)}_{L^2(\cD,\mu)}`
and the Cauchy-Schwarz inequality give the pointwise bound

.. math::
    :label: christoffel_bound

    |v(\vect{x})|^2 \leq k_m(\vect{x}) \norm{v}_{L^2(\cD,\mu)}^2.

The bound is sharp: :math:`k_m(\vect{x})` is the squared norm of the
evaluation functional at :math:`\vect{x}`, so

.. math::

    k_m(\vect{x}) = \sup_{\substack{v \in V_m \\ \norm{v}_{L^2(\cD,\mu)} = 1}}
    |v(\vect{x})|^2.

Both :math:`K_m` and :math:`k_m` depend only on the space :math:`V_m` and on
:math:`\mu`, not on the particular orthonormal basis: the sum of the squared
basis functions is the diagonal of the orthogonal projection kernel onto
:math:`V_m`. Integrating :eq:`christoffel_kernel` over :math:`\cD` and using
orthonormality gives the trace identity

.. math::
    :label: christoffel_trace

    \int_\cD k_m(\vect{x}) \di{\mu(\vect{x})}
    = \sum_{j=1}^m \norm{\varphi_j}_{L^2(\cD,\mu)}^2 = m.

.. _christoffel_distribution:

Christoffel distribution
------------------------

The Christoffel distribution :math:`\sigma_m` is the probability measure on
:math:`\cD` defined by

.. math::
    :label: eq_christoffel_distribution

    \di{\sigma_m(\vect{x})} = \frac{k_m(\vect{x})}{m} \di{\mu(\vect{x})},
    \qquad
    p_{\sigma_m}(\vect{x}) = p_\mu(\vect{x}) \frac{k_m(\vect{x})}{m}.

The trace identity :eq:`christoffel_trace` shows that :math:`\sigma_m` is a
probability measure. It is the reference measure tilted by the pointwise
energy of the approximation space: the density of :math:`\sigma_m` is large
where the basis functions are large, so that Christoffel samples concentrate
where the space has the largest pointwise influence. For the uniform measure
on :math:`[-1, 1]` and the Legendre basis, for example, the density
concentrates at the endpoints of the interval. For the arcsine measure and the
Chebyshev basis, the ratio :math:`k_m / m` stays bounded between two positive
constants, so :math:`\sigma_m` remains comparable to :math:`\mu` without
endpoint concentration.

The Christoffel measure is the optimal importance sampling measure for
:math:`L^2(\cD,\mu)` least squares. Let :math:`\nu` be any probability measure
on :math:`\cD` such that :math:`\mu` is absolutely continuous with respect to
:math:`\nu`, with Radon-Nikodym derivative :math:`w = \di{\mu} / \di{\nu}`. The
constant that enters the matrix concentration bound below is
:math:`\norm{w k_m}_{L^\infty(\nu)}`. Since

.. math::

    \int_\cD w(\vect{x}) k_m(\vect{x}) \di{\nu(\vect{x})}
    = \int_\cD k_m(\vect{x}) \di{\mu(\vect{x})} = m,

one has :math:`\norm{w k_m}_{L^\infty(\nu)} \geq m` for every :math:`\nu`,
with equality if and only if :math:`w k_m = m` :math:`\nu`-almost
everywhere, that is :math:`\nu = \sigma_m`. The Christoffel measure therefore
minimizes the sample size required by the concentration argument, and the
corresponding weights are

.. math::
    :label: christoffel_weights

    w(\vect{x}) = \frac{m}{k_m(\vect{x})}.

These weights are the density ratios of the reference measure over the
Christoffel measure. They turn a Christoffel sample into a quadrature rule for
:math:`\mu`: if :math:`\vect{X}_1, \dots, \vect{X}_n` are independent draws
from :math:`\sigma_m` and :math:`w_i = m / k_m(\vect{X}_i)`, then, for every
integrable function :math:`f`,

.. math::

    \Expect{\frac{1}{n} \sum_{i=1}^n w_i f(\vect{X}_i)}
    = \int_\cD f(\vect{x}) \di{\mu(\vect{x})}.

In particular, :math:`\frac{1}{n} \sum_{i=1}^n w_i v(\vect{X}_i)^2` is an
unbiased estimator of :math:`\norm{v}_{L^2(\cD,\mu)}^2` for every
:math:`v \in V_m`, and the normalized weighted Gramian

.. math::
    :label: christoffel_gramian

    \mat{G}_n = \frac{1}{n} \sum_{i=1}^n w_i
    \vect{\varphi}(\vect{X}_i) \Tr{\vect{\varphi}(\vect{X}_i)},
    \qquad
    \vect{\varphi}(\vect{x}) = (\varphi_1(\vect{x}), \dots, \varphi_m(\vect{x}))^\top,

satisfies :math:`\Expect{\mat{G}_n} = \mat{I}_m`. The condition number of
:math:`\mat{G}_n` controls the numerical stability of the weighted least
squares solve, and its deviation from the identity controls the statistical
error.

.. _christoffel_frame:

Weighted least squares and frame certificate
--------------------------------------------

Let :math:`g \in L^2(\cD, \mu)` be the function to approximate and let

.. math::

    \widetilde{g}_n = \argmin_{v \in V_m} \frac{1}{n} \sum_{i=1}^n
    w_i \left(g(\vect{X}_i) - v(\vect{X}_i)\right)^2

be the weighted least squares estimator on the design
:math:`(\vect{X}_i, w_i)_{1 \leq i \leq n}`. The design satisfies the frame
condition with constant :math:`\delta \in (0, 1)` when

.. math::
    :label: eq_christoffel_frame

    (1 - \delta) \norm{v}_{L^2(\cD,\mu)}^2
    \leq \frac{1}{n} \sum_{i=1}^n w_i v(\vect{X}_i)^2
    \leq (1 + \delta) \norm{v}_{L^2(\cD,\mu)}^2,
    \qquad \forall v \in V_m,

which is equivalent to :math:`\norm{\mat{G}_n - \mat{I}_m}_2 \leq \delta`.
For the Christoffel sampling measure, :math:`w k_m = m`, and the matrix
Chernoff bound of [cohendolbeault2020]_ gives, for every
:math:`\varepsilon \in (0, 1)`, the implication

.. math::
    :label: christoffel_budget

    n \geq \gamma m \log\left(\frac{2m}{\varepsilon}\right)
    \quad \Longrightarrow \quad
    \Prob{\norm{\mat{G}_n - \mat{I}_m}_2 \leq \frac{1}{2}} \geq 1 - \varepsilon,

with the constant

.. math::
    :label: christoffel_gamma

    \gamma = \left(\frac{3}{2} \log\left(\frac{3}{2}\right) - \frac{1}{2}\right)^{-1}
    \approx 9.242343873386666.

Under the same budget condition, the weighted least squares estimator is
near-best in expectation:

.. math::
    :label: christoffel_near_best

    \Expect{\norm{g - \widetilde{g}_n}_{L^2(\cD,\mu)}^2
    \mathbf{1}_{\norm{\mat{G}_n - \mat{I}_m}_2 \leq 1/2}}
    \leq (1 + \eta_n) e_m(g)^2,
    \qquad
    \eta_n = \frac{4}{n} \norm{w k_m}_{L^\infty(\nu)} = \frac{4m}{n}
    \leq \frac{4}{\gamma \log(2m/\varepsilon)},

where :math:`e_m(g) = \inf_{v \in V_m} \norm{g - v}_{L^2(\cD,\mu)}` is the
best approximation error in the space. The sampling error is thus a small
multiplicative factor over the best possible error, and the factor tends to
one as :math:`m` grows. The inequality also shows why the Christoffel measure
is optimal: it makes :math:`\norm{w k_m}_{L^\infty}` as small as possible,
hence the budget :math:`n` as small as possible.

The :class:`~openturns.experimental.ChristoffelSubsampleExperiment` uses the
largest integer :math:`m` such that :math:`n \geq \gamma m \log(m)` when its
certified mode is enabled. This is the leading-order form of
:eq:`christoffel_budget`, where the :math:`\log(2/\varepsilon)` term is
omitted. The experiment then draws :math:`n` independent points from
:math:`\sigma_m`, computes the weights :eq:`christoffel_weights`, and checks
the realized frame :eq:`eq_christoffel_frame` with the default tolerance
:math:`\delta = 1/2` (the ``ChristoffelSubsampleExperiment-FrameTolerance``
key). If the realized design misses the band, the whole independent sample is
redrawn, up to the number of attempts set by the
``ChristoffelSubsampleExperiment-MaxAttempts`` key. The last draw is returned
with a warning when no attempt passes. The theorem gives the probability
scale of the budget; the frame check turns it into an operational certificate
for the realized design.

The default mode of the experiment does not use this sizing: it chooses
:math:`m = n - k` as the largest dimension supplied by the basis, which gives
a much larger approximation space for the same budget but no certified frame
guarantee. The frame check is then advisory only. The trade-off is the
classical one between a square design (as many points as basis functions) and
an overdetermined certified design (more points than basis functions, with a
guaranteed stability margin).

.. _christoffel_dpp:

Determinantal point processes on the Christoffel kernel
-------------------------------------------------------

A determinantal point process on :math:`\cD` with kernel :math:`K` is a point
process whose joint intensities with respect to :math:`\mu` are determinants
of the kernel [macchi1975]_, [hough2009]_:

.. math::
    :label: dpp_intensity

    \rho_k(\vect{x}_1, \dots, \vect{x}_k)
    = \det\left[K(\vect{x}_i, \vect{x}_j)\right]_{1 \leq i, j \leq k},
    \qquad k \geq 1.

For the Christoffel kernel :math:`K_m` of :eq:`christoffel_kernel`, which is a
projection kernel of rank :math:`m`, the DPP has exactly :math:`m` points
almost surely. The joint density of the ordered configuration is

.. math::
    :label: dpp_density

    p(\vect{x}_1, \dots, \vect{x}_m)
    = \frac{1}{m!} \det\left[K_m(\vect{x}_i, \vect{x}_j)\right]_{1 \leq i, j \leq m}

with respect to :math:`\mu^{\otimes m}`; the normalization follows from the
Andreief identity and the trace identity :eq:`christoffel_trace`. The
one-point marginal density with respect to :math:`\mu` is
:math:`K_m(\vect{x}, \vect{x}) = k_m(\vect{x})`; normalized by the number of
points, the density of :math:`\sigma_m` with respect to :math:`\mu` is

.. math::

    \frac{\rho_1(\vect{x})}{m} = \frac{K_m(\vect{x}, \vect{x})}{m}
    = \frac{k_m(\vect{x})}{m},

and its Lebesgue density is
:math:`p_\mu(\vect{x}) k_m(\vect{x}) / m = p_{\sigma_m}(\vect{x})`.

A Christoffel sample is therefore marginally Christoffel-distributed, but the
points are not independent. The pair intensity is

.. math::
    :label: dpp_pair

    \rho_2(\vect{x}, \vect{y})
    = K_m(\vect{x}, \vect{x}) K_m(\vect{y}, \vect{y})
    - K_m(\vect{x}, \vect{y})^2
    \leq \rho_1(\vect{x}) \rho_1(\vect{y}),

with a strict inequality as soon as :math:`K_m(\vect{x}, \vect{y}) \neq 0`.
This negative dependence, or repulsion, is the defining feature of a DPP:
points avoid each other, and coincident points have probability zero for a
continuous :math:`\mu`. Repulsion is precisely what a design of experiments
needs: a DPP sample is spread over the support instead of clumping as an
independent sample would. DPP samples are also used directly as Monte Carlo
quadrature rules, with a variance controlled by the kernel [bardenet2020]_.

The DPP also has a volume interpretation. Let :math:`\mat{V}` be the
:math:`m \times m` matrix whose rows are
:math:`\Tr{\vect{\varphi}(\vect{x}_i)}`. Since :math:`K_m` is a projection
kernel,

.. math::
    :label: dpp_volume

    \det\left[K_m(\vect{x}_i, \vect{x}_j)\right]_{1 \leq i, j \leq m}
    = \det\left(\mat{V} \Tr{\mat{V}}\right)
    = \det\left(\Tr{\mat{V}} \mat{V}\right)
    = \left(\det \mat{V}\right)^2.

The probability of a configuration is thus proportional to the squared volume
of the parallelepiped spanned by its feature vectors. The DPP is the
continuous analogue of volume sampling: it favors configurations with a large
volume, hence well-spread points and a well-conditioned feature matrix. In the
univariate polynomial case, the projection DPP is the orthogonal polynomial
ensemble associated with :math:`\mu` [hough2009]_, the random matrix model
whose eigenvalues have the same Christoffel kernel.

.. _christoffel_dpp_sampling:

Sequential sampling
~~~~~~~~~~~~~~~~~~~

A projection DPP can be sampled exactly and sequentially. Suppose that
:math:`k` points have already been accepted, with feature matrix
:math:`\mat{V}_k` (one row per accepted point) and Gram matrix
:math:`\mat{G}_k = \mat{V}_k \Tr{\mat{V}_k}`. For a candidate
:math:`\vect{x}`, define

.. math::

    \vect{c}_k(\vect{x}) = \mat{V}_k \vect{\varphi}(\vect{x}).

The residual kernel after the :math:`k` accepted points is

.. math::
    :label: dpp_residual

    K_m^{(k)}(\vect{x}, \vect{y})
    = K_m(\vect{x}, \vect{y})
    - \Tr{\vect{c}_k(\vect{x})} \mat{G}_k^{-1} \vect{c}_k(\vect{y}),

and its diagonal :math:`r_k(\vect{x}) = K_m^{(k)}(\vect{x}, \vect{x})` is
the squared residual norm of the feature vector of :math:`\vect{x}` after
projection onto the span of the accepted features. The conditional density of
the next point is proportional to :math:`r_k(\vect{x})` with respect to
:math:`\mu`, with normalizing constant :math:`m - k`, because the residual
kernel has rank :math:`m - k` and trace :math:`m - k`.

The implementation proposes a candidate from the Christoffel distribution
:math:`\sigma_m`, whose density is :math:`K_m(\vect{x}, \vect{x}) / m`, and
accepts it with probability

.. math::
    :label: dpp_acceptance

    a_k(\vect{x}) = \frac{r_k(\vect{x})}{K_m(\vect{x}, \vect{x})}.

The acceptance probability is not the full target over proposal ratio: the
latter would be :math:`m r_k(\vect{x}) / ((m - k) K_m(\vect{x}, \vect{x}))`.
The missing constant cancels in the normalization of the rejection sampler,
so :eq:`dpp_acceptance` is exact and, unlike the full ratio, never exceeds
one. The expected number of proposals for the :math:`k`-th point is
:math:`m / (m - k)`, and the expected total number of proposals is

.. math::

    \sum_{k=0}^{m-1} \frac{m}{m-k} = m H_m = \cO(m \log m),

where :math:`H_m` is the :math:`m`-th harmonic number. Each accepted point
requires a rank-one update of the inverse Gram matrix (a Sherman-Morrison
update in the implementation), so the dense sequential sampler has a cost
that grows quickly with :math:`m`; this is why the experiment is intended for
moderate space dimensions. The implementation also guards against
non-finite or numerically zero residuals, which can occur when a candidate is
numerically in the span of the accepted points.

.. _christoffel_conditioning:

Conditional DPP and volume sampling
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Two variants are used when the design must reuse already computed points.

**Conditional DPP.** Let :math:`F = \{\vect{z}_1, \dots, \vect{z}_M\}` be a
set of supplied points with linearly independent feature vectors, and let
:math:`\mat{V}_F` be their :math:`M \times m` feature matrix. Conditionally on
these points being part of the DPP, the remaining :math:`m - M` points follow
a DPP with the residual kernel

.. math::
    :label: dpp_conditional

    K_F(\vect{x}, \vect{y})
    = K_m(\vect{x}, \vect{y})
    - \Tr{\vect{c}_F(\vect{x})} \left(\mat{V}_F \Tr{\mat{V}_F}\right)^{-1}
    \vect{c}_F(\vect{y}),
    \qquad
    \vect{c}_F(\vect{x}) = \mat{V}_F \vect{\varphi}(\vect{x}).

This is the same residual kernel as in :eq:`dpp_residual`, with the fixed
points playing the role of the accepted points. Conditioning is possible only
for at most :math:`m` supplied points; more than :math:`m` points cannot be
conditioned on because the projection kernel has rank :math:`m`, and the
feature vectors of supplied points must be linearly independent for
:math:`\mat{V}_F \Tr{\mat{V}_F}` to be invertible.

**Volume sampling from a candidate set.** Let
:math:`Z = \{\vect{z}_1, \dots, \vect{z}_M\}` be a candidate set with
:math:`M \geq n`, drawn from the reference measure :math:`\mu`, and let
:math:`\vect{v}_i = \vect{\varphi}(\vect{z}_i)` be its feature vectors. Volume
sampling selects a subset :math:`S \subset \{1, \dots, M\}` of size :math:`n`
with probability

.. math::
    :label: volume_sampling

    \Prob{S} \propto \det\left(\Tr{\mat{V}_S} \mat{V}_S\right),

where :math:`\mat{V}_S` is the :math:`n \times m` matrix of the selected
feature vectors. When :math:`n = m`, this is exactly the projection DPP on
the discrete measure :math:`(1/M) \sum_{i=1}^M \delta_{\vect{z}_i}`. The
selection law is the DPP one precisely because the candidates are reference
draws: the selected points inherit the Christoffel law and the weights
:math:`m / k_m` turn the subset into a quadrature rule for :math:`\mu`. When
:math:`M = n`, the selection is the identity: all candidates are kept and,
since no Christoffel selection has taken place, they are returned with unit
weights as a plain Monte Carlo sample of :math:`\mu`. When
:math:`n > m`, the determinant is positive exactly when the selected feature
vectors span the :math:`m`-dimensional space, and it measures the squared
volume of their parallelepiped; volume sampling extends the DPP construction
by favoring subsets whose span has a large volume [derezinski2021]_. It is
the natural way to recycle a larger pool of already evaluated points, and it
is exact, not a heuristic thinning.

The implementation draws the volume sample by reverse removal. With
:math:`r` candidates remaining, the leverage score of candidate :math:`i` is

.. math::

    h_i = \Tr{\vect{v}_i} \left(\Tr{\mat{V}} \mat{V}\right)^{-1} \vect{v}_i,

where :math:`\mat{V}` is the current :math:`r \times m` feature matrix. Since
:math:`\sum_{i=1}^r (1 - h_i) = r - m`, removing candidate :math:`i` with
probability proportional to :math:`1 - h_i` yields exactly the volume
distribution :eq:`volume_sampling` after :math:`M - n` removals. The
Gramian inverse and the leverage scores are updated by Sherman-Morrison after
each removal, with a periodic exact refresh to control round-off errors. A
zero removal probability signals a numerically rank-deficient candidate set
and is rejected.

**Mixing a DPP with independent points.** In the default square mode with
:math:`n > m`, the first :math:`m` points form a projection DPP and the
remaining :math:`n - m` points are independent draws from
:math:`\sigma_m`. The union is not a DPP: the independent part adds no
repulsion, and the two blocks are not exchangeable. The experiment randomly
permutes the design so that the returned ordering does not expose the blocks,
and it checks the frame only as an advisory diagnostic. This construction
improves the budget of the square design while keeping the exact DPP for the
part that carries the space dimension.

.. _christoffel_sampling_algorithms:

Sampling the Christoffel distribution
-------------------------------------

The Christoffel distribution is the reference measure tilted by
:math:`k_m`, and its support is the support of :math:`\mu`. The implementation
provides two sampling paths, described in
:class:`~openturns.experimental.ChristoffelDistribution`.

**Tensor-product reference measure.** When :math:`\mu` is a product measure
with independent marginals, :math:`\mu = \bigotimes_{j=1}^\inputDim \mu_j`,
and the basis is a tensor product of univariate functions, the Christoffel
density factorizes conditionally. Write a basis function as
:math:`\varphi_i(\vect{x}) = \prod_{j=1}^\inputDim \psi_{i_j}(x_j)`, where
:math:`i = (i_1, \dots, i_\inputDim)` is a multi-index. Given the first
:math:`j - 1` coordinates, the conditional density of :math:`X_j` is

.. math::
    :label: christoffel_conditional

    p_j(t \mid x_1, \dots, x_{j-1})
    = \frac{p_{\mu_j}(t) \, S_j(t)}{N_j},
    \qquad
    S_j(t) = \sum_i \left(\prod_{l < j} \psi_{i_l}(x_l)^2\right) \psi_{i_j}(t)^2,
    \qquad
    N_j = \sum_i \prod_{l < j} \psi_{i_l}(x_l)^2.

This conditional density is sampled by rejection. A bounded marginal uses a
uniform proposal on its numerical range and an envelope of the target
density. An unbounded marginal uses a two-sided Cauchy proposal, or a
half-Cauchy proposal for a one-sided support, because the polynomial factor
:math:`S_j` gives the Christoffel density heavier tails than the reference
measure: truncating at the numerical range would bias high-degree samples.
The envelope is scanned on a grid uniform in the proposal CDF, which resolves
narrow peaks near the mode, and multiplied by a safety factor. A proposal
that exceeds the envelope raises it and is discarded, so the sampler never
accepts a proposal above the envelope. As for any rejection sampler,
exactness is conditional on the envelope bounding the target-to-proposal
ratio everywhere: the scan is a numerical estimate, not a proof of the
supremum, so a peak missed by the grid can bias the sample. Since an accepted
draw is final, raising the envelope after a later exceedance cannot correct a
draw accepted under an earlier, too low envelope.

**Other reference measures.** When the tensor-product structure is not
available, the class compares two samplers before generating and keeps the
one with the larger measured acceptance rate. The first is rejection from
the reference measure with the envelope :math:`K_m^\star`, an estimate of
:math:`\sup_{\cD} k_m` over the numerical range obtained from a
low-discrepancy candidate pool and refined by a bounded multi-start
maximization of the log-Christoffel function, with a safety factor.
Rejection accepts with probability :math:`m / K_m^\star`. The second is the
internal ratio-of-uniforms sampler
[stadlober1990]_. A setup failure at any dimension falls back to rejection.
The stability factor carries a safety margin, the rejection sampler raises
its envelope and discards the offending proposal whenever one exceeds it, and
the ratio-of-uniforms sampler rejects every proposal that violates its bounds.
As on the tensor path, rejection is exact only when the envelope bounds the
density ratio: the stability factor remains a numerical estimate, and a later
exceedance cannot correct an earlier accepted draw.
The ratio-of-uniforms sampler operates on the numerical range, so it is exact
for a bounded reference whose range covers the support and can truncate the
tails of an unbounded non-tensor reference; use the tensor path for unbounded
product measures.

Two numerical cautions are worth stating. First, the interval returned by
``getRange()`` is a numerical range, not necessarily the support: for an
unbounded reference it stores extreme quantiles, and the PDF may be positive
outside that interval. The tensor sampler does not truncate an unbounded
marginal to the numerical range, while the ratio-of-uniforms branch is set up
on that range. Second, a reference density that underflows to zero makes the
PDF zero even when the reference log-density is finite; the log-PDF and the
log-Christoffel function are the stable entry points in the tails.

**Orthonormality is a precondition.** The Christoffel function is a density
factor only when the basis is orthonormal with respect to :math:`\mu`. The
classes do not check this condition. A cheap numerical check is the trace
identity :eq:`christoffel_trace`, for example the empirical mean of
``computeChristoffelFunction()`` over a sample of :math:`\mu`, which must be
close to :math:`m`. For a correlated reference measure, tensor-orthonormal
functions must be composed with a whitening map of :math:`\mu` before being
passed to the basis: for a multivariate normal, use
:math:`\vect{x} \mapsto \mat{L}^{-1} \vect{x}` where :math:`\mat{L}` is the
Cholesky factor of the correlation matrix.

.. topic:: API:

    - :class:`~openturns.experimental.ChristoffelDistribution`
    - :class:`~openturns.experimental.ChristoffelSubsampleExperiment`

.. topic:: Examples:

    - See :doc:`/auto_design_of_experiments/plot_christoffel_subsample`
    - See :doc:`/auto_design_of_experiments/plot_christoffel_ishigami`

.. topic:: References:

    - [bardenet2020]_
    - [cohendolbeault2020]_
    - [derezinski2021]_
    - [hough2009]_
    - [macchi1975]_
