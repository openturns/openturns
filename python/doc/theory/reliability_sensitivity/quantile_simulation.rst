.. _quantile_simulation_theory:

=====================================
Quantile estimation by simulation
=====================================

Let :math:`\inputRV` be a random vector with distribution
:math:`\inputMeasure` on :math:`\Rset^{\inputDim}` and let
:math:`\model:\Rset^{\inputDim}\to\Rset` be a measurable scalar function.
The output is

.. math::

    Y = \model(\inputRV).

We assume that the distribution function :math:`F` of :math:`Y` has a
quantile function

.. math::
    :label: eq-quantile-definition

    q_\alpha
    = F^{-1}(\alpha)
    = \inf\{y\in\Rset : F(y)\ge \alpha\},
    \qquad \alpha\in(0,1).

For a continuous and strictly increasing :math:`F`, the quantile
:math:`q_\alpha` is the unique solution of :math:`F(q_\alpha)=\alpha`.
For a general :math:`F`, definition :eq:`eq-quantile-definition` selects
the smallest value whose distribution function reaches :math:`\alpha`.
When :math:`F` is continuous at :math:`q_\alpha`, the upper-tail
probability is

.. math::

    p = 1-\alpha,

so that :math:`q_\alpha` is the value exceeded with probability
:math:`p`.  When :math:`F` has an atom at :math:`q_\alpha`, the
exceedance probability :math:`\Prob{Y>q_\alpha}=1-F(q_\alpha)` is
smaller than :math:`p`.  Throughout this page, :math:`\alpha` denotes the
*quantile level* and :math:`\beta` denotes the *confidence level*;
the two numbers are independent.

The quantile simulation algorithms estimate :math:`q_\alpha` from a
finite number of evaluations of :math:`\model`.  They differ in the
sampling mechanism, in the estimator, in the confidence interval and in
the coverage guarantee that the interval can legitimately claim.  This
page presents the common statistical background and then the theory of
each family of algorithms.

.. note::
    The weighted-experiment estimator
    (:class:`~openturns.experimental.MonteCarloQuantileSimulation`) is the
    only quantile simulation algorithm available in this release.  The
    other algorithms described below are documented for reference and will
    be added in forthcoming releases.

.. _quantile_theory_definitions:

Estimators and coverage
-----------------------

A quantile simulation algorithm returns an estimator, not the
theoretical quantile.  Given an i.i.d. sample
:math:`Y_1,\dots,Y_N` of :math:`Y`, let

.. math::

    Y_{(1)} \le Y_{(2)} \le \cdots \le Y_{(N)}

be the order statistics.  The empirical inverse estimator is

.. math::
    :label: eq-empirical-inverse

    \widehat{q}_\alpha^{\mathrm{inv}}
    = Y_{(\lceil N\alpha\rceil)}.

The interpolated (Hyndman--Fan type 5) estimator is

.. math::
    :label: eq-type5

    \widehat{q}_\alpha^{\mathrm{type5}}
    = (1-\gamma)Y_{(k)} + \gamma Y_{(k+1)},
    \qquad
    h = N\alpha + \frac12,\quad
    k = \lfloor h\rfloor,\quad
    \gamma = h-k,

with the obvious clamping at the boundaries.  The empirical inverse is
an order statistic and therefore admits the exact distribution-free
interval recalled below.  The interpolated estimator is intended to
reduce the leading bias on smooth densities, but the reduction is not
universal; it is not an order statistic and the exact binomial interval
does not apply verbatim.

The coverage vocabulary used by
:class:`~openturns.experimental.QuantileSimulationResult` distinguishes
four guarantees:

- **Exact coverage.**  For every supported sample size :math:`N`,
  :math:`\Prob{q_\alpha\in\CI}\ge\beta` unconditionally, with
  no asymptotic approximation.  This requires an i.i.d. sample, uniform
  weights, the empirical inverse and a sample size fixed before the
  observations are drawn.
- **Asymptotic coverage.**
  :math:`\Prob{q_\alpha\in\CI}\to\beta` as the relevant sample
  size, number of blocks, number of replicates or number of directions
  tends to infinity.
- **Anytime-valid coverage.**
  :math:`\Prob{\forall k\ge0,\ q_\alpha\in\CI_{n_k}}\ge\beta`
  for a data-dependent stopping time on the deterministic candidate
  sizes :math:`n_0<n_1<\dots`.  This is the guarantee of a confidence
  sequence, not of a fixed-sample interval evaluated at a random time.
- **No coverage.**  The algorithm returns a point estimate and possibly
  a diagnostic interval, but no frequentist guarantee.

Exact order-statistic intervals
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`B` be the number of observations not exceeding the true
quantile,

.. math::

    B = \sum_{i=1}^N \Ind{Y_i\le q_\alpha}.

If :math:`F` is continuous at :math:`q_\alpha`, each indicator has
probability :math:`F(q_\alpha)=\alpha`, so

.. math::

    B \sim \Bin(N,\alpha).

For :math:`1\le r_1<r_2\le N`, the interval
:math:`[Y_{(r_1)},Y_{(r_2)}]` then satisfies

.. math::
    :label: eq-exact-coverage

    \Prob{Y_{(r_1)}\le q_\alpha\le Y_{(r_2)}}
    = \Prob{r_1\le B\le r_2-1}
    = F_{\Bin}(r_2-1;N,\alpha)
      - F_{\Bin}(r_1-1;N,\alpha).

The coverage depends only on :math:`N`, :math:`\alpha`, :math:`r_1` and
:math:`r_2`, not on :math:`F`.  This is Wilks' classical result
[wilks1948]_.  The ranks are computed by
:class:`~openturns.QuantileConfidence`.  When no finite pair of ranks
reaches the requested coverage, the minimum sample size is reported and
the algorithm must not return an interval with a smaller coverage under
the nominal label.

If :math:`F` has an atom at :math:`q_\alpha`, the indicator probability
is :math:`F(q_\alpha)\ge\alpha`, not :math:`\alpha`.  The binomial
parameter is therefore :math:`F(q_\alpha)`, and
:eq:`eq-exact-coverage` must be read as a lower bound: the ranks
computed with parameter :math:`\alpha` give each one-sided bound a
coverage of at least its nominal value, so the bilateral interval is
conservative.  The interval remains distribution-free, but its actual
coverage depends on the size of the atom, and the quantile itself may
be non-unique; definition :eq:`eq-quantile-definition` selects the
lower quantile.  A unilateral interval has an infinite endpoint when
its rank falls outside :math:`\llbracket 1,N\rrbracket`; substituting
the sample minimum or maximum would invalidate the coverage statement.

Asymptotic intervals
~~~~~~~~~~~~~~~~~~~~

Assume that :math:`F` is twice differentiable in a neighborhood of
:math:`q_\alpha`, with a bounded second derivative and density
:math:`f(q_\alpha)>0`.  The Bahadur representation of the empirical
quantile [bahadur1966]_ gives

.. math::

    \widehat{q}_\alpha - q_\alpha
    = \frac{\alpha-\widehat{F}_N(q_\alpha)}{f(q_\alpha)}
      + \cO_{\Pset}\left(N^{-3/4}(\log N)^{3/4}\right),

and therefore

.. math::
    :label: eq-quantile-clt

    \sqrt{N}\left(\widehat{q}_\alpha-q_\alpha\right)
    \xrightarrow{\cL}
    \cN\left(0,
      \frac{\alpha(1-\alpha)}{f(q_\alpha)^2}\right).

The asymptotic variance is unknown because the density is unknown.  A
spacing estimator of the density at the quantile is

.. math::
    :label: eq-spacing

    \widehat{f}(q_\alpha)
    = \frac{r_2-r_1}{N\left(Y_{(r_2)}-Y_{(r_1)}\right)},

for ranks :math:`r_1<r_2` around :math:`\lceil N\alpha\rceil`.  The
corresponding Wald interval is

.. math::
    :label: eq-wald

    \widehat{q}_\alpha
    \pm z_{(1+\beta)/2}
      \frac{\sqrt{\alpha(1-\alpha)}}
           {\sqrt{N}\,\widehat{f}(q_\alpha)},

where :math:`z_{(1+\beta)/2}` is the standard normal quantile.

The Woodruff interval avoids the density estimate by inverting a
confidence interval for :math:`F(q_\alpha)` [woodruff1952]_:

.. math::
    :label: eq-woodruff

    \left[
      \widehat{F}_N^{-1}\left(
        \alpha-z_{(1+\beta)/2}
        \sqrt{\frac{\alpha(1-\alpha)}{N}}\right),
      \;
      \widehat{F}_N^{-1}\left(
        \alpha+z_{(1+\beta)/2}
        \sqrt{\frac{\alpha(1-\alpha)}{N}}\right)
    \right],

with the probability arguments clamped to :math:`(0,1)`.  The interval
is asymptotically valid and is the default for weighted samples, where
the density at the quantile is difficult to estimate.

Weighted samples
~~~~~~~~~~~~~~~~

Importance sampling draws :math:`\inputRV_i` from a proposal density
:math:`h` and computes :math:`Y_i=\model(\inputRV_i)` with weights

.. math::

    w_i = \frac{f_{\inputRV}(\inputRV_i)}{h(\inputRV_i)},

or, in the standard space, the Radon--Nikodym derivative of the target
measure with respect to the proposal.  The weighted empirical
distribution function is the ratio estimator

.. math::
    :label: eq-weighted-cdf

    \widehat{F}_N(y)
    = \frac{\sum_{i=1}^N w_i\,\Ind{Y_i\le y}}
           {\sum_{i=1}^N w_i},

and the weighted empirical inverse is

.. math::
    :label: eq-weighted-inverse

    \widehat{q}_\alpha = Y_{(K)},
    \qquad
    K = \inf\left\{k : \sum_{i=1}^k W_{(i)}\ge\alpha\right\},

where the outputs are sorted together with their normalized weights
:math:`W_{(i)}=w_{(i)}/\sum_j w_j`.

The asymptotic variance of :math:`\widehat{F}_N(y)` at a fixed
:math:`y` is estimated by

.. math::
    :label: eq-weighted-variance

    \widehat{V}_N(y)
    = \frac{\sum_{i=1}^N w_i^2
        \left(\Ind{Y_i\le y}-\widehat{F}_N(y)\right)^2}
           {\left(\sum_{i=1}^N w_i\right)^2},

and the effective sample size is the Kish quantity

.. math::
    :label: eq-ess

    \ESS
    = \frac{\left(\sum_{i=1}^N w_i\right)^2}
           {\sum_{i=1}^N w_i^2}.

The weighted Woodruff interval is

.. math::
    :label: eq-weighted-woodruff

    \left[
      \widehat{F}_N^{-1}\left(
        \alpha-z_{(1+\beta)/2}\sqrt{\widehat{V}_N(\widehat{q}_\alpha)}
      \right),
      \;
      \widehat{F}_N^{-1}\left(
        \alpha+z_{(1+\beta)/2}\sqrt{\widehat{V}_N(\widehat{q}_\alpha)}
      \right)
    \right].

It is asymptotic and requires independent observations: the variance
estimator :eq:`eq-weighted-variance` sums the contribution of each draw
separately and omits every covariance term.  It is therefore valid for
i.i.d. draws from the proposal, as produced by
:class:`~openturns.ImportanceSamplingExperiment`, and not for a
dependent randomized design such as a randomized low-discrepancy
sequence or a determinantal design.  The weights must also have a
finite second moment and the proposal must dominate the target over its
whole support, not only on the relevant tail: when the proposal vanishes
where the target has mass, the self-normalized weighted empirical
distribution converges to the target restricted to the proposal support,
and its quantiles are biased.  Heavy-tailed weights make :math:`\ESS` small and the
normal approximation poor.  When independence is not guaranteed, the
interval is a diagnostic and the result must carry ``COVERAGE_NONE``.

.. _quantile_theory_weighted:

Weighted-experiment estimator
-----------------------------

:class:`~openturns.experimental.MonteCarloQuantileSimulation` is the
reference quantile estimator driven by a
:class:`~openturns.WeightedExperiment`.  The experiment produces a
design :math:`\inputRV_1,\dots,\inputRV_N` and non-negative weights
:math:`w_1,\dots,w_N`; the algorithm evaluates
:math:`Y_i=\model(\inputRV_i)` and applies the weighted empirical
inverse :eq:`eq-weighted-inverse`.  The weights enter the empirical
distribution function :eq:`eq-weighted-cdf`, never the output values:
multiplying the outputs by the weights would estimate the quantile of a
different random variable.

The estimator is selected by ``setEstimatorType``.  The default,
``EMPIRICAL_INVERSE``, is :eq:`eq-weighted-inverse` and returns an
order statistic.  The ``INTERPOLATED`` alternative returns a convex
combination of two neighbouring order statistics: with uniform weights
it uses the type-5 (Hazen) plotting-position rule of [hyndman1996]_,
and with non-uniform weights it interpolates linearly in the cumulative
weight.  The interpolated value is not an order statistic, so the
exact binomial interval :eq:`eq-exact-coverage` does not apply: the
algorithm then falls back to the replicate Student interval or to the
weighted Woodruff interval, according to the design.

The confidence interval and the coverage label depend on the
experiment:

- i.i.d. sampling with uniform weights and the empirical inverse: the
  exact binomial order-statistic interval :eq:`eq-exact-coverage`,
  with coverage ``COVERAGE_EXACT``;
- a randomized uniform-weight design that is not i.i.d., for example a
  randomized low-discrepancy sequence or a Latin hypercube design:
  :math:`R` independent replicates and the Student interval
  :eq:`eq-replicate-t`, with coverage ``COVERAGE_ASYMPTOTIC``;
- an importance-sampling experiment with non-uniform weights: the
  weighted Woodruff interval :eq:`eq-weighted-woodruff`, with coverage
  ``COVERAGE_ASYMPTOTIC``, because its i.i.d. proposal draws make the
  weighted variance estimator valid;
- any other weighted design: the weighted Woodruff interval
  :eq:`eq-weighted-woodruff`, with coverage ``COVERAGE_NONE``, because
  the weighted variance estimator omits the covariance terms.

The replicate interval is

.. math::
    :label: eq-replicate-t

    \widehat{q}_\alpha
    \pm t_{R-1,(1+\beta)/2}\,\frac{s_q}{\sqrt{R}},

where :math:`\widehat{q}_\alpha` is the mean of the :math:`R`
independent replicate estimates and :math:`s_q` is their sample
standard deviation.  As :math:`R\to\infty`, this interval covers the
expectation of the replicate estimator, not necessarily
:math:`q_\alpha`.  Covering :math:`q_\alpha` additionally requires the
per-replicate bias to be negligible relative to the standard error
:math:`s_q/\sqrt{R}`.  Increasing :math:`R` alone does not reduce that
bias: with a one-point Latin hypercube on :math:`\mathcal{U}(0,1)` and
:math:`\alpha=0.9`, every replicate estimate is uniform and the
interval concentrates on :math:`1/2`.  The coverage label
``COVERAGE_ASYMPTOTIC`` therefore refers to the joint limit
:math:`R\to\infty` with a design size large enough for the bias to
vanish; for a fixed design size the interval is a diagnostic, not a
guarantee for :math:`q_\alpha`.

For a deterministic design there is no sampling distribution: the
reported interval describes the design itself, not a repeated-sampling
guarantee, and the result must carry ``COVERAGE_NONE``.  A single
randomized quasi-Monte Carlo sequence is a dependent sample, so the
exact binomial ranks do not apply; independent randomizations are
required for the replicate interval.  Likewise, a Latin hypercube
experiment that reuses its stratum permutation from one draw to the
next (``alwaysShuffle=false``) produces correlated replicates and is
rejected.

.. _quantile_theory_block:

Memory-bounded estimators
-------------------------

The full-sample estimator stores :math:`N` outputs.  For large
simulation budgets this is often the binding constraint.  Two
strategies reduce the memory without giving up the quantile estimate.

Block estimator
~~~~~~~~~~~~~~~

:class:`~openturns.experimental.BlockQuantileSimulation` splits the
stream of :math:`N` independent outputs into :math:`B` blocks of length
:math:`b`, with :math:`N=Bb`.  Let :math:`\widehat{q}_i` be the
empirical quantile of block :math:`i`.  The block quantiles are i.i.d.
and are combined into one estimate:

.. math::
    :label: eq-block-combination

    \begin{aligned}
    \widehat{q}^{\mathrm{mean}}
      &= \frac{1}{B}\sum_{i=1}^B \widehat{q}_i,\\
    \widehat{q}^{\mathrm{med}}
      &= \median
         \left(\widehat{q}_1,\dots,\widehat{q}_B\right),\\
    \widehat{q}^{\mathrm{trim}}
      &= \frac{1}{B-2\lfloor\gamma B\rfloor}
         \sum_{i=\lfloor\gamma B\rfloor+1}^{B-\lfloor\gamma B\rfloor}
         \widehat{q}_{(i)},
    \end{aligned}

where :math:`\widehat{q}_{(1)}\le\cdots\le\widehat{q}_{(B)}` are the
sorted block quantiles and :math:`\gamma` is the trimming fraction.  The
mean has the smallest asymptotic variance, the median is robust to
heavy-tailed block quantiles, and the trimmed mean is a compromise.

The default confidence interval is the Student interval

.. math::
    :label: eq-block-t

    \widehat{q}
    \pm t_{B-1,(1+\beta)/2}\,\frac{s_q}{\sqrt{B}},

where :math:`s_q` is the sample standard deviation of the block
quantiles.  Each block quantile has variance of order :math:`b^{-1}`
and bias of order :math:`b^{-1}`, so the mean combination has variance
of order :math:`(Bb)^{-1}=N^{-1}`, independent of the block length,
and remains biased at order :math:`b^{-1}`.  The Student interval
covers the mean block quantile, hence :math:`q_\alpha`, only when the
bias is negligible relative to the standard error of order
:math:`N^{-1/2}`; this requires :math:`b` to grow faster than
:math:`\sqrt{N}`.  The choice :math:`b\asymp\sqrt{N}` balances the
bias against the standard error and minimizes the :math:`O(b+N/b)`
memory, but does not make the bias negligible: the result must carry
``COVERAGE_NONE`` for that policy.  Choosing :math:`b\asymp N^{2/3}`
makes the bias :math:`o(N^{-1/2})` and is the usual choice when a
coverage statement is wanted.  A percentile bootstrap interval is
available as an alternative to :eq:`eq-block-t`.

The standard error :math:`s_q/\sqrt{B}` in :eq:`eq-block-t` is the one
of the mean combination.  For the median, it is multiplied by
:math:`\sqrt{\pi/2}`, the asymptotic ratio between the standard errors
of the sample median and the sample mean for normal block quantiles.
For the trimmed mean, let :math:`\lfloor\gamma B\rfloor` values be
removed from each side and let :math:`w_1\le\dots\le w_B` be the
winsorized block quantiles, obtained by replacing the removed values by
the first and last retained values.  The Tukey--McLaughlin standard
error is

.. math::

    \frac{s_w}
         {\left(1-2\lfloor\gamma B\rfloor/B\right)\sqrt{B}},

where :math:`s_w` is the sample standard deviation of the winsorized
values [tukey1963]_.

The block length is a statistical parameter.  It is distinct from the
evaluation block size, which only controls how many points are evaluated
between progress steps.

Exact-tail estimator
~~~~~~~~~~~~~~~~~~~~

:class:`~openturns.experimental.ExactTailQuantileSimulation` computes
the exact binomial interval of a tail quantile without storing the
full sample.  With :math:`B\sim\Bin(N,\alpha)` and zero-based
ranks :math:`0\le r_1<r_2<N` returned by
:class:`~openturns.QuantileConfidence`, the interval

.. math::

    \left[Y_{(r_1+1)},Y_{(r_2+1)}\right]

has the exact distribution-free coverage
:math:`\Prob{r_1+1\le B\le r_2}\ge\beta`.  The point estimate is the
empirical inverse :math:`Y_{(\lceil N\alpha\rceil)}`.

For an upper-tail quantile only the :math:`N-r_1` largest
observations are needed; for a lower-tail quantile only the
:math:`r_2+1` smallest observations are needed.  Keeping the smaller
tail in a bounded heap gives a memory of

.. math::

    K \approx N(1-\alpha)
      + z_{(1+\beta)/2}\sqrt{N\alpha(1-\alpha)}

for an upper-tail quantile.  The estimate and the interval are exactly
the order statistics of the full sample, with no approximation.  The
interpolated estimator is rejected because its value is not an order
statistic and the exact binomial interval would no longer apply.

.. _quantile_theory_rare_event:

Rare-event estimators
---------------------

For a small upper-tail probability :math:`p=1-\alpha`, plain Monte
Carlo requires a sample of order :math:`1/p` to observe the target
tail.  Subset simulation, the last-particle algorithm and cross-entropy
importance sampling construct the tail progressively and estimate the
quantile from the resulting population or weighted sample.

Subset simulation
~~~~~~~~~~~~~~~~~

``SubsetSamplingQuantile`` uses the
fixed-level splitting scheme of [au2001]_.  Write :math:`p=1-\alpha`
for the upper tail (:math:`p=\alpha` for the lower tail) and let
:math:`p_0\in(0,1)` be the conditional probability.  The number of
levels is fixed before the run:

.. math::
    :label: eq-subset-levels

    m = \left\lceil\frac{\log p}{\log p_0}\right\rceil.

At level :math:`j=1,\dots,m-1`, the algorithm computes the
:math:`(1-p_0)`-quantile :math:`t_j` of the current population for the
upper tail (the :math:`p_0`-quantile for the lower tail), so that the
upper tail keeps a fraction :math:`p_0` of the particles, and replaces
the particles that do not satisfy :math:`Y>t_j` (respectively
:math:`Y<t_j` for the lower tail) by draws from the conditional
distribution given the same event.  The replacement uses a
componentwise random-walk Metropolis--Hastings kernel in the standard
space with a finite burn-in [cerou2007]_.

The final population is approximately distributed from :math:`Y`
conditioned on the last threshold.  The target conditional tail
probability at level :math:`m-1` is

.. math::
    :label: eq-subset-pcond

    p_{\mathrm{cond}}
    = \frac{p}{p_0^{m-1}}
    \in [p_0,1),

and the quantile estimate is the empirical order statistic of the final
conditional sample at level :math:`1-p_{\mathrm{cond}}` (respectively
:math:`p_{\mathrm{cond}}` for the lower tail) [guyader2011]_.

The whole procedure is repeated :math:`R` times with independent
streams.  The reported estimate is the mean of the replicate estimates
and the interval is the Student interval :eq:`eq-replicate-t`, whose
coverage is asymptotic in :math:`R`.  The conditional samples are
produced by a Markov chain and the thresholds are data-dependent, so
the exact conditional rank interval is not an unconditional exact
interval for :math:`q_\alpha`; the result carries
``COVERAGE_ASYMPTOTIC``.

Last-particle algorithm
~~~~~~~~~~~~~~~~~~~~~~~

``LastParticleQuantile`` is the limiting
case of subset simulation in which the conditional probability is
:math:`p_0=1-1/N` [guyader2011]_.  Exactly one particle is replaced at
each level, the level probability is no longer a tuning parameter, and
the number of moves needed to reach the target tail has an exact
Poisson law in the idealized process with independent conditional
draws.

Let :math:`p=1-\alpha` for the upper tail and let
:math:`(\inputRV_i,Y_i)_{i=1}^N` be an initial i.i.d. population.  Each
move selects the particle with the smallest output, draws its input
from the distribution conditioned on exceeding the current extreme
output with a Metropolis--Hastings kernel and a finite burn-in, and
replaces the particle.  The lower tail is handled symmetrically by
reversing the selection and the comparison.

Let :math:`K` be the first move whose threshold crosses the true
quantile, so that :math:`t_k` lies on the same side of the quantile for
:math:`k<K` and on the other side for :math:`k\ge K`.  In the idealized
process, :math:`K` is Poisson with mean :math:`\lambda=-N\log p`.  The
algorithm runs to the upper Poisson rank

.. math::
    :label: eq-last-particle-rank

    U = \left\lceil
      Q_{\Pois(\lambda)}
      \left(\frac{1+\beta}{2}\right)\right\rceil,

and reports Walter's reduced-bias midpoint estimate [walter2015]_,

.. math::
    :label: eq-last-particle-estimate

    \widehat{q}_\alpha = \frac{t_{K_0-1}+t_{K_0}}{2},
    \qquad K_0 = \left\lceil -N\log p\right\rceil,

where :math:`t_k` is the threshold of move :math:`k`.  The interval
:math:`[t_L,t_U]` covers the quantile exactly when :math:`L<K\le U`, so
the lower rank is one less than the Poisson quantile at level
:math:`(1-\beta)/2`, while the upper rank is the Poisson quantile at
level :math:`(1+\beta)/2`:

.. math::

    L = \max\left(1,\,
      Q_{\Pois(\lambda)}\left(\frac{1-\beta}{2}\right)-1\right),
    \qquad
    U = \max\left(L,\,
      Q_{\Pois(\lambda)}\left(\frac{1+\beta}{2}\right)\right).

The one-based rank convention clamps :math:`L` to one, whereas the exact
lower endpoint would be :math:`-\infty` when the lower Poisson quantile
is zero or one.  The supported range :math:`N\ge 10` and
:math:`p\le 1/2` keeps the lower Poisson quantile at least two, so the
clamp is not active.  For the lower tail, the interval endpoints are
reversed.  The coverage is exact for the idealized process with
independent conditional draws; the finite-burn-in Markov chain makes the
achieved coverage asymptotic, so the result carries
``COVERAGE_ASYMPTOTIC``.

Cross-entropy importance sampling
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``CrossEntropyQuantile`` estimates the
quantile by importance sampling with a cross-entropy auxiliary
distribution [rubinstein2017]_.  A pilot sample first locates the target
tail; the event :math:`\{Y>t_0\}` (respectively :math:`\{Y<t_0\}`) is
then passed to a standard-space cross-entropy run with intermediate
quantile level :math:`\rho` [deboer2005]_.  The run produces an
auxiliary Gaussian distribution
:math:`q_{\mathrm{CE}}=\cN(\vect{\mu},\vect{\sigma})` in the
standard space.

For a standard Gaussian target and a centered Gaussian auxiliary
distribution with scales :math:`\sigma_i`, the second moment of the
importance weights :math:`\varphi/q` is finite only when
:math:`\sigma_i^2>1/2` in every direction, because the integrand then
decays faster than the proposal.  Below that threshold the second
moment is infinite and the self-normalized average is dominated by a
few draws.  The estimator therefore uses the defensive proposal

.. math::
    :label: eq-defensive-proposal

    q = \cN(\vect{\mu},\bds),
    \qquad
    s_i = \max(\sigma_i,s_{\min}),
    \qquad
    s_{\min}\ge 1,

which keeps the cross-entropy location and inflates every scale to at
least one.  The floor gives a finite second moment and a safety margin.

With the standard normal density :math:`\varphi` of the input
dimension, the importance weights are

.. math::

    w_i = \frac{\varphi(\RVU_i)}{q(\RVU_i)},

and the weighted empirical inverse :eq:`eq-weighted-inverse` gives the
estimate.  The confidence interval is the weighted Woodruff interval
:eq:`eq-weighted-woodruff`, with the variance
:eq:`eq-weighted-variance` evaluated at the estimate.  Its coverage is
asymptotic and degrades when the effective sample size
:eq:`eq-ess` becomes small.

.. _quantile_theory_directional:

Directional, line and analytical estimators
-------------------------------------------

When the limit state :math:`\model(\inputRV)=t` has a differentiable
geometry, the probability of exceeding a threshold can be estimated by
integrating a conditional probability over directions or lines.  The
quantile is then the root of a monotone probability equation.

Directional sampling
~~~~~~~~~~~~~~~~~~~~

``DirectionalSamplingQuantile`` works in
the standard space.  For a direction :math:`\realU` on the unit sphere
and a threshold :math:`t`, let :math:`r(\realU,t)` be the distance
along :math:`\realU` at which the limit state
:math:`\model(r\realU)=t` is reached.  The formula below assumes that
the exceedance set is the outer radial interval along each direction,
i.e. :math:`\model(r'\realU)>t` for :math:`r'>r(\realU,t)` and
:math:`\model(r'\realU)\le t` below it.  This holds when the limit
state is crossed exactly once along the ray and the failure domain is
star-shaped as seen from the origin.  The conditional probability
that the output exceeds :math:`t` in that direction is
[bjerager1988]_

.. math::
    :label: eq-directional-conditional

    \ProbCond{\RVU=\realU}{Y>t}
    = 1-F_{\chi_d}\left(r(\realU,t)^2\right),

where :math:`F_{\chi_d}` is the chi-square distribution function with
:math:`d` degrees of freedom.  The directional estimate of
:math:`\Prob{Y>t}` is the average of
:eq:`eq-directional-conditional` over a sample of directions.

The quantile solves

.. math::
    :label: eq-directional-root

    H(t) = \Expect{1-F_{\chi_d}\left(r(\RVU,t)^2\right)} - p
    = 0,
    \qquad p=1-\alpha.

The same direction sample is used at every candidate :math:`t`
(common random numbers), so that the estimated :math:`H` is a smooth
random function of :math:`t` and a bisection or secant iteration
converges to the solution of :math:`\widehat{H}_N(t)=0` rather than
oscillating on independent noise.  The interval follows from the delta
method applied to the conditional contributions, or from independent
replicates and the interval :eq:`eq-replicate-t`; its coverage is
asymptotic.  If the root is not valid or does not converge, the result
carries ``COVERAGE_NONE``.

Line sampling
~~~~~~~~~~~~~

``LineSamplingQuantile`` uses an important
direction :math:`\bde` fixed before the root-finding loop
[pradlwarter2007]_.  For a threshold :math:`t`, the conditional
probability of exceeding :math:`t` on a line is a normal tail
probability, and the line estimate averages it over the lines:

.. math::
    :label: eq-line-estimate

    \widehat{\Pset}(Y>t)
    = \frac{1}{N}\sum_{i=1}^N
      \Phi\left(-r(\realU_i,t)\right),

where :math:`\Phi` is the standard normal distribution function and
:math:`r(\realU_i,t)` is the signed distance to the limit state along
line :math:`i`.  The quantile is again the root of
:math:`\widehat{\Pset}(Y>t)-p=0`, solved with common random lines.  The
important direction is not updated across thresholds, because doing so
would break the common-random-number coupling that makes the outer
objective well defined.  The interval and the coverage label are the
same as for directional sampling.

Post-analytical estimators
~~~~~~~~~~~~~~~~~~~~~~~~~~

``PostAnalyticalQuantile`` replaces the
sampling probability by the analytical FORM or SORM probability.  For a
candidate threshold :math:`t`, the event :math:`\{\model(\inputRV)>t\}`
is analysed in the standard space, the design point and the Hasofer
reliability index are computed, and the analytical probability
:math:`P_{\mathrm{FORM/SORM}}(t)` is evaluated.  The quantile is the
deterministic root of

.. math::
    :label: eq-post-analytical-root

    P_{\mathrm{FORM/SORM}}(t)=1-\alpha.

The analytical probability is a smooth deterministic function of
:math:`t`, so the root is bracketed and bisected at negligible cost.
FORM and SORM are approximations, not sampling guarantees, so the pure
analytical quantile carries ``COVERAGE_NONE``.

The analytical quantile can be refined by importance sampling around
the design point [hohenbichler1988]_.  Each of the :math:`R` replicates
draws an independent sample, solves the weighted empirical root with
that fixed sample, and the result is the mean of the replicate
estimates with the Student interval :eq:`eq-replicate-t`.  The refined
path carries ``COVERAGE_ASYMPTOTIC`` when at least two replicates are
available.

Probability inversion
~~~~~~~~~~~~~~~~~~~~~

``ProbabilityInversionQuantile`` is a
generic adapter for probability engines that expose only a scalar
probability for a threshold event.  Let :math:`p=1-\alpha` for an
upper-tail quantile and

.. math::
    :label: eq-inversion-problem

    P(t) = \Prob{Y>t},
    \qquad h(t) = P(t)-p.

Assume that :math:`F` is continuous and strictly increasing near
:math:`q_\alpha`, so that :math:`P` is continuous and strictly
decreasing with :math:`P(q_\alpha)=p`.  The adapter solves
:math:`h(t)=0`.  For a lower-tail quantile it replaces :math:`Y>t` by
:math:`Y<t` and :math:`p` by :math:`\alpha`; the event operator is
carried by the wrapped engine, so the same loop handles both tails.

When the wrapped engine is a plain Monte Carlo engine with a
:class:`~openturns.MonteCarloExperiment`, the adapter delegates to
:class:`~openturns.experimental.MonteCarloQuantileSimulation` and returns
the exact order-statistic interval.  Other engines are inverted by a
confidence-bound bisection: an endpoint is discarded only when its whole
confidence bound lies on one side of the target, and the iteration stops
when the bracket is narrow or when the bound straddles the target.  The
adapter cannot claim common random numbers unless the wrapped engine
exposes an injectable random stream; in the absence of such a stream the
result carries ``COVERAGE_NONE``.  A bracket failure raises an
exception: a boundary root or a flat probability surface is never
reported as a silent point estimate.

.. _quantile_theory_sequential:

Two-stage and sequential estimators
-----------------------------------

A fixed-sample exact interval cannot be used after an optional stopping
rule.  If the algorithm stops when the interval becomes shorter than a
target, the stopping time depends on the same observations that enter
the interval, and the coverage of the reported interval is generally
below :math:`\beta`.  This is the classical optional-stopping problem;
the quantile version is discussed in [chow1965]_ and [howard2022]_.
There are two clean remedies.

Two-stage design
~~~~~~~~~~~~~~~~

``TwoStageQuantileSimulation`` uses a pilot
sample to choose the final sample size, then draws an independent
confirmation sample and computes the exact order-statistic interval on
that sample only.

1. **Pilot.**  Draw :math:`N_0` i.i.d. observations.  Compute the exact
   bilateral order-statistic interval of the pilot and let :math:`h_0`
   be its half-width.  The pilot interval width estimates
   :math:`z_{(1+\beta)/2}\sqrt{\alpha(1-\alpha)}/
   (f(q_\alpha)\sqrt{N_0})` with much less variance than a
   single-spacing density estimate.
2. **Sizing.**  Choose the confirmation size from the pilot half-width
   so that the expected half-width of the exact interval targets
   :math:`h`:

   .. math::
       :label: eq-two-stage-size

       N_1 = \left\lceil N_0\left(\frac{h_0}{h}\right)^2\right\rceil
       \approx
       \left\lceil
         \frac{z_{(1+\beta)/2}^2\,\alpha(1-\alpha)}
              {f(q_\alpha)^2\,h^2}
       \right\rceil.

   The pilot half-width is random, so this is an estimated target rather
   than a guarantee: a pilot that underestimates the width can lead to a
   confirmation interval wider than :math:`h`.

   When the pilot is too small for an exact interval, the asymptotic
   form with the spacing density :eq:`eq-spacing` is used instead.  The
   size is clamped from below by the minimum sample size of the exact
   bilateral interval and from above by the remaining evaluation budget.
3. **Confirmation.**  Draw an independent sample of size :math:`N_1`,
   compute the empirical inverse :eq:`eq-empirical-inverse` and the exact
   interval :eq:`eq-exact-coverage` on that sample only.

Because :math:`N_1` is a function of the pilot sample, which is
independent of the confirmation sample, and because :math:`N_1` is fixed
before the confirmation sample is drawn, the exact coverage is preserved
unconditionally, pilot randomness included.  The pilot observations do
not enter the estimate or the interval.  If the budget is insufficient
to reach the target half-width, the confirmation sample is limited to
the available budget, the result records the budget exhaustion and a
warning is logged; it must not silently return an interval with the
nominal target width.

Sequential design
~~~~~~~~~~~~~~~~~

``SequentialQuantileSimulation`` uses an
anytime-valid confidence sequence and stops when the interval is short
enough or when the budget is exhausted.  The distribution-free version
maintains a time-uniform band for :math:`F` from the
Dvoretzky--Kiefer--Wolfowitz--Massart inequality
[dvoretzky1956]_ [massart1990]_:

.. math::
    :label: eq-dkw

    \Prob{\sup_y\left|\widehat{F}_n(y)-F(y)\right|>\varepsilon_n}
    \le 2e^{-2n\varepsilon_n^2}.

The band is evaluated on a deterministic geometric grid of candidate
sample sizes :math:`n_k`.  At grid index :math:`k`, the failure
probability is

.. math::
    :label: eq-sequential-delta

    \delta_k = \frac{1-\beta}{(k+1)(k+2)},
    \qquad
    \varepsilon_k = \sqrt{\frac{\log(2/\delta_k)}{2n_k}}.

Because :math:`\sum_{k\ge0}\delta_k=1-\beta`, the intervals hold
simultaneously at every candidate size with probability at least
:math:`\beta`.  The union bound is what makes the interval valid at the
data-dependent stopping time.

The band is inverted into a quantile interval by

.. math::
    :label: eq-sequential-inversion

    q_\alpha \in
    \left[
      \inf\{y : \widehat{F}_n(y)\ge\alpha-\varepsilon_n\},
      \;
      \inf\{y : \widehat{F}_n(y)>\alpha+\varepsilon_n\}
    \right],

which is evaluated with the order statistics of the sample.  An endpoint
is infinite when :math:`\alpha\pm\varepsilon_n` leaves :math:`(0,1)`;
the result reports the corresponding infinite bound.  The algorithm
stops when the interval half-width reaches the target or when the
evaluation budget is reached, and the result records the stopping time,
the stopping criterion and the full confidence-sequence path.  The
coverage is ``COVERAGE_ANYTIME_VALID``.

A group-sequential design with Pocock or O'Brien--Fleming boundaries is
valid only at the pre-specified interim analyses; it is not
anytime-valid and must not carry ``COVERAGE_ANYTIME_VALID``.
