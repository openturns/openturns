%feature("docstring") OT::QuantileConfidence
R"RAW(Estimate confidence intervals of a quantile.

Refer to :any:`quantile_confidence_estimation`.

Parameters
----------
alpha : float
    Quantile level.
beta : float, optional
    Confidence level. Default value is 0.95.

Notes
-----
This class estimates bounds of the quantile of level
:math:`\alpha \in [0,1]` of the random variable :math:`X`
with a confidence greater than :math:`\beta` using order statistics.

Let :math:`x_{\alpha}` be the unknown quantile of level :math:`\alpha`
of the random variable :math:`X` of dimension 1.
Let :math:`(X_0, \dots, X_{\sampleSize - 1})` be a sample of independent
and identically distributed variables according to :math:`X`.

We now introduce the order statistics used to compute the quantile bounds.
Let :math:`X_{(k)}` be the :math:`k+1` -th order statistics of
:math:`(X_0, \dots, X_{\sampleSize - 1})` for :math:`0 \leq k \leq \sampleSize - 1`.
For example, :math:`X_{(0)} = \min (X_0, \dots, X_{\sampleSize - 1})`
and :math:`X_{(\sampleSize - 1)} = \max (X_0, \dots, X_{\sampleSize - 1})`.

By definition of order statistics, we have:

.. math::

    X_{(0)} \leq X_{(1)} \leq \dots \leq X_{(\sampleSize - 1)}.

Note: Python uses 0-based indexing.
Therefore, the theoretical order statistic :math:`X_{(k)}`
(with :math:`1 \leq k \leq \sampleSize`) corresponds to :math:`X_{(k - 1)}`
in this class.

Examples
--------
In the next example, we compute a confidence interval
for the quantile of level 95% with confidence 95%.
More precisely, we compute an upper bound of this quantile.

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> alpha = 0.95
>>> beta = 0.95
>>> sample = ot.Gumbel().getSample(100)
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> ci = algo.computeUnilateralConfidenceInterval(sample)
>>> upper_bound = ci.getUpperBound()[0]
>>> print(upper_bound)
3.999...
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeUnilateralRank
R"RAW(Evaluate the rank of a quantile lower or upper bound.

If `lower_bounded` is `True`, the lower rank :math:`k_{low}` is the largest rank :math:`k` with
:math:`0 \leq k \leq \sampleSize - 1` such that:

.. math::

    \Prob{X_{(k)} \leq x_{\alpha}} \geq \beta.

In other words, the interval :math:`\left[ X_{(k_{low})}, +\infty\right)` is a
unilateral confidence interval for the quantile :math:`x_\alpha` with
confidence :math:`\beta`.

If `lower_bounded` is `False`, the upper rank :math:`k_{up}` is the smallest rank :math:`k` with
:math:`0 \leq k \leq \sampleSize - 1` such that:

.. math::

    \Prob{x_{\alpha} \leq X_{(k)}} \geq \beta.

In other words, the interval :math:`\left(-\infty, X_{(k_{up})}\right]` is a
unilateral confidence interval for the quantile :math:`x_\alpha` with confidence
:math:`\beta`.

Refer to :any:`quantile_confidence_estimation` for the conditions under
which a solution exists, and the solution itself.

Parameters
----------
size : int
    Sample size
lower_bounded : bool, optional
    False to compute the upper bound of the quantile,
    True for the lower bound.
    Default value is False.

Returns
-------
rank : int
    Rank :math:`k`.

Examples
--------
In the next example, we consider a sample which size is 100.
We compute the rank of the observation such that :math:`x_{(k)}`
is a lower bound of the quantile of order :math:`\alpha`, with confidence
:math:`\beta`.
See [meeker2017]_, Table J.11, page 548 for comparison.

>>> import openturns as ot
>>> alpha = 0.05
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> lower_bounded = True
>>> rank = algo.computeUnilateralRank(300, lower_bounded)
>>> print(rank)
8
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeBilateralRank
R"RAW(Evaluate the ranks of a quantile bilateral bound.

The ranks :math:`k_1, k_2` with :math:`0 \leq k_1 \leq k_2 \leq \sampleSize - 1`
are defined by:

.. math::

    \begin{array}{ll}
    (k_1, k_2) = 
    & \argmin \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}}\\
    & \mbox{s.t.} \; \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}} \geq \beta
    \end{array}

In other words, the interval :math:`\left[ X_{(k_1)}, X_{(k_2)} \right]` is a
bilateral confidence interval for the quantile :math:`x_\alpha` with confidence
:math:`\beta`.

Refer to :any:`quantile_confidence_estimation` for the conditions under
which a solution exists, and the solution itself.

Parameters
----------
size : int
    Sample size :math:`\sampleSize`.

Returns
-------
ranks : :class:`~openturns.Indices` of size 2
    Pair of lower and upper ranks :math:`(k_1, k_2)`.

Examples
--------
>>> import openturns as ot
>>> alpha = 0.05
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> k1, k2 = algo.computeBilateralRank(100)
>>> print((k1, k2))
(1, 10)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeUnilateralConfidenceInterval
R"RAW(Evaluate a unilateral confidence interval of a quantile.

If `lower_bounded` is `True`, the lower-bounded confidence interval is given
by the order statistic:

.. math::

    \left[X_{(k)}, +\infty\right)

where :math:`k` is the largest rank such that
:math:`0 \leq k \leq \sampleSize - 1` and:

.. math::

    \Prob{X_{(k)} \leq x_{\alpha}} \geq \beta.

If `lower_bounded` is `False`, the upper-bounded confidence interval is given
by the order statistic:

.. math::

    \left(-\infty, X_{(k)}\right]

where :math:`k` is the smallest rank such that
:math:`0 \leq k \leq \sampleSize - 1` and:

.. math::

    \Prob{x_{\alpha} \leq X_{(k)}} \geq \beta.

The rank :math:`k` is evaluated by :meth:`computeUnilateralRank`.

The order statistic is computed from the provided sample.

Parameters
----------
sample : 2-d sequence of float
    Sample of the variable :math:`X`.
lower_bounded : bool, optional
    False to compute the upper bound of the quantile,
    True for the lower bound.
    Default value is False.

Returns
-------
ci : :class:`~openturns.Interval`
    Quantile confidence interval.

Examples
--------
In the next example, we compute a confidence interval
for the quantile of level 95% with confidence 95%.
More precisely, we compute an upper bound of this quantile.

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> alpha = 0.95
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> sample = ot.Gumbel().getSample(100)
>>> ci = algo.computeUnilateralConfidenceInterval(sample)
>>> print(ci.getUpperBound()[0])
3.999...
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeUnilateralConfidenceIntervalWithCoverage
R"RAW(Evaluate a unilateral confidence interval of a quantile.

Parameters
----------
sample : 2-d sequence of float
    Sample of the variable :math:`X`.
lower_bounded : bool, optional
    False to compute the upper bound of the quantile,
    True for the lower bound.
    Default value is False.

Returns
-------
ci : :class:`~openturns.Interval`
    Quantile confidence interval.

coverage : float
    If `lower_bounded` is True, then the `coverage` is the value of
    :math:`\Prob{X_{(k)} \leq x_{\alpha}}`.
    Otherwise, the `coverage` is the value of :math:`\Prob{x_{\alpha} \leq X_{(k)}}`.
    In both cases, the coverage is guaranteed to be greater or equal to
    :math:`\beta`.

Notes
-----
Refer to :func:`computeUnilateralConfidenceInterval()`.

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> alpha = 0.95
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> sample = ot.Gumbel().getSample(100)
>>> ci, coverage = algo.computeUnilateralConfidenceIntervalWithCoverage(sample)
>>> print(coverage)
0.962...
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeBilateralConfidenceInterval
R"RAW(Evaluate a bilateral confidence interval of a quantile.

The confidence interval for the quantile of level :math:`\alpha` is given by
the order statistics:

.. math::

    [X_{(k_1)}, X_{(k_2)}]

where :math:`(k_1, k_2)`  are the ranks with
:math:`0 \leq k_1 \leq k_2 \leq \sampleSize - 1` defined by:

.. math::

    \begin{array}{ll}
    (k_1, k_2) = 
    & \argmin \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}}\\
    & \mbox{s.t.} \; \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}} \geq \beta
    \end{array}

The ranks :math:`(k_1, k_2)` are given by :meth:`computeBilateralRank`.

The statistics values are evaluated from the given sample.

Parameters
----------
sample : 2-d sequence of float
    Sample of the variable :math:`X`.

Returns
-------
ci : :class:`~openturns.Interval`
    Quantile confidence interval.

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> alpha = 0.05
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> sample = ot.Gumbel().getSample(60)
>>> ci = algo.computeBilateralConfidenceInterval(sample)
>>> print(ci)
[-1.310..., -0.532...]
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeBilateralConfidenceIntervalWithCoverage
R"RAW(Evaluate a bilateral confidence interval of a quantile.

Refer to :func:`computeBilateralConfidenceInterval()`

Parameters
----------
sample : 2-d sequence of float
    Sample of the variable :math:`X`.

Returns
-------
ci : :class:`~openturns.Interval`
    Quantile confidence interval.

coverage : float
    The `coverage` is the value of
    :math:`\Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}}`.
    The coverage is guaranteed to be greater or equal to :math:`\beta`.

Notes
-----
Refer to :func:`computeBilateralConfidenceInterval()`.

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> alpha = 0.05
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> sample = ot.Gumbel().getSample(60)
>>> ci, coverage = algo.computeBilateralConfidenceIntervalWithCoverage(sample)
>>> print(coverage)
0.9510...
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeUnilateralMinimumSampleSize
R"RAW(Evaluate the minimum sample size for the unilateral confidence interval.

Let :math:`k \geq 0` be the given (tail) rank using 0-indexing.
The method computes the smallest sample size :math:`\sampleSize` required to
guarantee a confidence level :math:`\beta` for the specified unilateral
confidence interval of the quantile :math:`x_\alpha`.

If `lower_bounded` is `True`, the condition to satisfy for the lower-bounded
confidence interval :math:`\left[ X_{(k)}, +\infty\right)` is:

.. math::

    \Prob{X_{(k)} \leq x_{\alpha}} \geq \beta.

If `lower_bounded` is `False`, the condition to satisfy for the upper-bounded
confidence interval :math:`\left(-\infty, X_{(\sampleSize - k - 1)} \right]` is:

.. math::

    \Prob{x_{\alpha} \leq X_{(\sampleSize - k - 1)}} \geq \beta.

Notice that the actual rank used for the upper bound is
:math:`\sampleSize - k - 1`.

A notable feature of this method is its use of the tail rank to compute the
minimum sample size.
This allows the evaluation of an arbitrary order statistic, rather than being
restricted to the extreme observations, i.e.,
:math:`X_{(0)}` or :math:`X_{(\sampleSize - 1)}`.
While this is straightforward when computing the lower confidence bound,
computing the upper confidence bound requires the adjusted rank
:math:`\sampleSize - k - 1`. This corresponds to the rank counted from the
largest observation in the sample.

Refer to :any:`quantile_confidence_estimation` for the conditions under
which a solution exists, and the solution itself.

Parameters
----------
tail_rank : int, optional, :math:`k \geq 0`
    The (tail) rank of the quantile, using 0-indexing.
    Default value is 0.
lower_bounded : bool, optional
    False to compute the upper bound of the quantile,
    True for the lower bound.
    Default value is False.

Returns
-------
size : int
    Minimum sample size :math:`\sampleSize`.

Examples
--------
In the next example, we compute the smallest sample size such that the
largest observation in the sample, i.e. :math:`x_{(\sampleSize - 1)}`, is an
upper bound of the quantile of order :math:`\alpha`, with confidence
:math:`\beta`.
See [meeker2017]_, Table J.13, page 549 for comparison.

>>> import openturns as ot
>>> alpha = 0.95
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> size = algo.computeUnilateralMinimumSampleSize(0)
>>> print(size)
59

Consider an upper confidence bound based on a given tail rank, for instance
:math:`k = 10`.
The goal is to verify that the condition
:math:`\Prob{x_{\alpha} \leq X_{(\sampleSize - k - 1)}} \geq \beta` is satisfied.
Note that because we are evaluating an upper bound, the actual rank must be
computed from the tail rank as :math:`\sampleSize - k - 1`.

>>> alpha = 0.95
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> lower_bounded = False
>>> tail_rank = 10
>>> size = algo.computeUnilateralMinimumSampleSize(tail_rank, lower_bounded)
>>> print(size)
336
>>> rank = size - tail_rank - 1
>>> coverage = algo.computeUnilateralCoverage(size, rank, lower_bounded)
>>> print(coverage)
0.9503...
>>> print(coverage >= beta)
True

Consider a lower confidence bound based on a given tail rank, for instance
:math:`k = 10`.
The goal is to verify that the condition
:math:`\Prob{X_{(k)} \leq x_{\alpha}} \geq \beta` is satisfied.
Note that because we are evaluating a lower bound, the actual rank used for the
order statistic is exactly the tail rank :math:`k`.

>>> alpha = 0.05
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> lower_bounded = True
>>> tail_rank = 10
>>> size = algo.computeUnilateralMinimumSampleSize(tail_rank, lower_bounded)
>>> print(size)
336
>>> rank = tail_rank
>>> coverage = algo.computeUnilateralCoverage(size, rank, lower_bounded)
>>> print(coverage)
0.9503...
>>> print(coverage >= beta)
True
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeBilateralMinimumSampleSize
R"RAW(Evaluate the minimum size of the sample for the bilateral confidence interval.

The method determines the smallest sample size :math:`\sampleSize` so that:

.. math::

    \Prob{X_{(0)} \leq x_{\alpha} \leq X_{(\sampleSize - 1)}} \geq \beta.

In other words, the interval :math:`\left[X_{(0)}, X_{(\sampleSize - 1)} \right]`
is a bilateral confidence interval for the quantile :math:`x_\alpha` with
confidence :math:`\beta`, where :math:`X_{(0)} = \min(X_0, \dots, X_{\sampleSize - 1})`
and :math:`X_{(\sampleSize - 1)} = \max(X_0, \dots, X_{\sampleSize - 1})`.

Refer to :any:`quantile_confidence_estimation` for the conditions under
which a solution exists, and the solution itself.

Returns
-------
size : int
    Minimum sample size :math:`\sampleSize`.

Examples
--------
>>> import openturns as ot
>>> alpha = 0.05
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> size = algo.computeBilateralMinimumSampleSize()
>>> print(size)
59
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeAsymptoticBilateralRank
R"RAW(Evaluate the asymptotic bilateral rank of a quantile.

This method computes two integers :math:`(k_1, k_2)` such that
:math:`0 \leq k_1 \leq k_2 \leq \sampleSize - 1` and:

.. math::

    \lim_{\sampleSize \rightarrow \infty} \Prob{X_{(k_1)} \leq x_\alpha \leq X_{(k_2)}}
    = \beta.

In other words, the interval :math:`\left[X_{(k_1)}, X_{(k_2)} \right]` is an
asymptotic confidence interval for :math:`x_\alpha` with asymptotic confidence
:math:`\beta`.
The ranks :math:`k_1, k_2 \in \llbracket 0, \sampleSize - 1 \rrbracket` are
estimated from:

.. math::

    k_1 & = \left\lfloor \sampleSize \alpha - z_{\gamma} \sqrt{\sampleSize \alpha (1 - \alpha)} \right\rfloor - 1 \\
    k_2 & = \left\lfloor \sampleSize \alpha + z_{\gamma} \sqrt{\sampleSize \alpha (1 - \alpha)} \right\rfloor - 1

where :math:`\gamma \in [0,1]` is defined as:

.. math::

    \gamma = 1 - \frac{1 - \beta}{2}

where :math:`z_{\gamma}` is the standard Gaussian quantile of order :math:`\gamma`.
See [delmas2006]_ proposition 12.2.13 page 257 for more details on this topic.

Parameters
----------
size : int
    Sample size

Returns
-------
ranks : :class:`~openturns.Indices` of size 2
    Pair of lower and upper ranks :math:`(k_1, k_2)` with
    :math:`0 \leq k_1 \leq k_2 \leq \sampleSize - 1`.

Examples
--------
>>> import openturns as ot
>>> alpha = 0.05
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> k1, k2 = algo.computeAsymptoticBilateralRank(100)
>>> print((k1, k2))
(0, 8)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeAsymptoticBilateralConfidenceInterval
R"RAW(Evaluate asymptotic bilateral bounds of a quantile.

The asymptotic bounds are given by the order statistics:

.. math::

    [X_{(k_1)}, X_{(k_2)}]

so that:

.. math::

    \lim_{\sampleSize \rightarrow \infty}
    \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}} \geq \beta

where the ranks :math:`0 \leq k_1 \leq k_2 \leq \sampleSize - 1` are given by
:meth:`computeAsymptoticBilateralRank`.

Parameters
----------
sample : 2-d sequence of float
    Sample of the variable :math:`X`.

Returns
-------
ci : :class:`~openturns.Interval`
    Quantile confidence interval.

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> alpha = 0.05
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> sample = ot.Gumbel().getSample(60)
>>> ci = algo.computeAsymptoticBilateralConfidenceInterval(sample)
>>> print(ci)
[-1.310..., -0.898...]
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::setAlpha
"Quantile level accessor.

Parameters
----------
alpha : float
    Quantile level.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::getAlpha
"Quantile level accessor.

Returns
-------
alpha : float
    Quantile level
"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::setBeta
"Confidence level accessor.

Parameters
----------
beta : float
    Confidence level.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::getBeta
"Confidence level accessor.

Returns
-------
beta : float
    Confidence level
"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeUnilateralCoverage
R"RAW(Evaluate the coverage probability of a unilateral confidence interval.

This method computes the exact probability that the true quantile of level
:math:`\alpha` belongs to the unilateral confidence interval defined by the
order statistic of the given rank. The computation relies on the Binomial
distribution and does not require a sample.

If `lower_bounded` is True, the method computes:

.. math::

    \Prob{X_{(k)} \leq x_{\alpha}}

If `lower_bounded` is False, the method computes:

.. math::

    \Prob{x_{\alpha} \leq X_{(k)}}

Parameters
----------
size : int
    Sample size :math:`\sampleSize`.
rank : int
    Rank :math:`k` of the order statistic.
lower_bounded : bool, optional
    False to consider the upper bound of the quantile,
    True for the lower bound.
    Default value is False.

Returns
-------
coverage : float
    Probability of coverage.

Examples
--------
>>> import openturns as ot
>>> alpha = 0.05
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> lower_bounded = True
>>> coverage = algo.computeUnilateralCoverage(300, 8, lower_bounded)
>>> print(coverage)
0.9659...
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileConfidence::computeBilateralCoverage
R"RAW(Evaluate the coverage probability of a bilateral confidence interval.

This method computes the exact probability that the true quantile of level
:math:`\alpha` belongs to the bilateral confidence interval defined by the
order statistics of the given ranks. The computation relies on the Binomial
distribution and does not require a sample.

The method computes:

.. math::

    \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}}

Parameters
----------
size : int
    Sample size :math:`\sampleSize`.
rank1 : int
    Lower rank :math:`k_1`.
rank2 : int
    Upper rank :math:`k_2`.

Returns
-------
coverage : float
    Probability of coverage.

Examples
--------
>>> import openturns as ot
>>> alpha = 0.05
>>> beta = 0.95
>>> algo = ot.QuantileConfidence(alpha, beta)
>>> coverage = algo.computeBilateralCoverage(100, 1, 10)
>>> print(coverage)
0.9514...
)RAW"
