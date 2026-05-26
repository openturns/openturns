.. _quantile_confidence_estimation:

Exact quantile confidence interval based on order statistics
------------------------------------------------------------

We consider a random variable :math:`X` of dimension 1 and its quantile
:math:`x_{\alpha}` of level :math:`\alpha` (:math:`\alpha \in [0, 1]`).
We seek to evaluate an upper bound of :math:`x_{\alpha}` with a confidence
greater than or equal to :math:`\beta`, using order statistics.

Let :math:`(X_1, \dots, X_\sampleSize)` be independent copies of :math:`X`.
Let :math:`X_{(k)}` be the :math:`k`-th order statistic of
:math:`(X_1, \dots, X_\sampleSize)` i.e., :math:`X_{(k)}` is the
:math:`k`-th smallest value of :math:`(X_1, \dots, X_\sampleSize)` for :math:`1 \leq k \leq \sampleSize`.
For example, :math:`X_{(1)} = \min (X_1, \dots, X_\sampleSize)` is the minimum
and :math:`X_{(\sampleSize)} = \max (X_1, \dots, X_\sampleSize)` is the maximum.
We have:

.. math::

    X_{(1)} \leq X_{(2)} \leq \dots \leq X_{(\sampleSize)}.

The cumulative distribution function of the order statistic :math:`X_{(k)}` is:

.. math::

    F_{X_{(k)}}(x)
    = \sum_{i = k}^{\sampleSize} \binom{\sampleSize}{i}
    \left(F(x)\right)^i \left(1 - F(x) \right)^{\sampleSize-i}.

For any strictly positive integer :math:`\sampleSize` and :math:`\alpha \in [0, 1]`,
let :math:`F_{(\sampleSize, \alpha)}` be the cumulative distribution function of
the binomial distribution :math:`\cB(\sampleSize, \alpha)`.
Let :math:`\overline{F}` be the complementary cumulative distribution function
(also named survival function in dimension 1):

.. math::

    \overline{F}_{(\sampleSize, \alpha)}(k)
    = 1 - F_{(\sampleSize, \alpha)}(k),

for any integer :math:`k`.
Then the cumulative distribution function of the :math:`k`-th order statistic
is:

.. math::

    F_{X_{(k)}}(x) = \overline{F}_{(\sampleSize,F(x))}(k - 1),

for any integer :math:`k \in \llbracket 1, \sampleSize \rrbracket`.
Therefore:

.. math::

    \begin{aligned}
    F_{X_{(k)}}(x_{\alpha})
    & = \sum_{i = k}^{\sampleSize} \binom{\sampleSize}{i} \alpha^i (1 - \alpha)^{\sampleSize - i} \\
    & = \overline{F}_{(\sampleSize, \alpha)}(k - 1).
    \end{aligned}

Rank for an upper bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`(x_1, \dots, x_\sampleSize)` be an i.i.d. sample of size
:math:`\sampleSize` of the random variable :math:`X`.
Given a quantile level :math:`\alpha \in [0,1]`, a confidence level
:math:`\beta \in [0,1]`, and a sample size :math:`\sampleSize`, we seek the
smallest rank :math:`k \in \llbracket 1, \sampleSize \rrbracket` such that:

.. math::
    :label: EqOrderStatB

    \Prob{x_{\alpha} \leq X_{(k)}} \geq \beta.

The previous equation is equivalent to:

.. math::
    :label: EqOrderStat2B

    F_{(\sampleSize, \alpha)}(k - 1) \geq \beta.

Then, the smallest rank :math:`k_{\text{sol}}` such that the previous equation
is satisfied is:

.. math::

    k_{\text{sol}}
    = \min \{ k \in \llbracket 1, \sampleSize \rrbracket \,
            \mid \, F_{(\sampleSize, \alpha)}(k - 1)\geq \beta \}.

If:

.. math::

    1 - \alpha^\sampleSize < \beta,

then there is no solution.
Otherwise, the solution :math:`k_{\text{sol}}` is:

.. math::
    :label: EqOrderStatBgen

    k_{\text{sol}} = 1 + F_{(\sampleSize, \alpha)}^{-1}(\beta).

First, the optimal rank :math:`k_{\text{sol}}` is computed.
Then, the upper bound of :math:`x_{\alpha}` is estimated using the corresponding
order statistic :math:`x_{(k_{\text{sol}})}` from the sample
:math:`(x_1, \dots, x_\sampleSize)`.

The following table presents the solution depending on the parameters.

+------------------------+------------------+----------------------------+--------------------+
| :math:`k_{\text{sol}}` | :math:`\beta=0`  | :math:`0 < \beta < 1`      | :math:`\beta=1`    |
+========================+==================+============================+====================+
| :math:`\alpha=0`       | 1                | 1                          | 1                  |
+------------------------+------------------+----------------------------+--------------------+
| :math:`0 < \alpha < 1` | 1                | see :eq:`EqOrderStatBgen`  | :math:`\emptyset`  |
+------------------------+------------------+----------------------------+--------------------+
| :math:`\alpha=1`       | 1                | :math:`\emptyset`          | :math:`\emptyset`  |
+------------------------+------------------+----------------------------+--------------------+

Rank for a lower bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given the same data as previously, we seek the greatest rank
:math:`k \in \llbracket 1, \sampleSize \rrbracket` such that:

.. math::
    :label: EqOrderStatA

    \Prob{X_{(k)} \leq x_{\alpha}} \geq \beta.

This is equivalent to:

.. math::

    \overline{F}_{(\sampleSize, \alpha)}(k - 1) \geq \beta.

Then, the greatest rank :math:`k_{\text{sol}}` such that the previous equation
is satisfied is:

.. math::

    k_{\text{sol}}
    = \max \left\{ k \in \llbracket 1, \sampleSize \rrbracket \,
             \mid \, \overline{F}_{(\sampleSize, \alpha)}(k - 1) \geq \beta \right\}.

If:

.. math::

    1 - (1 - \alpha)^\sampleSize < \beta,

then there is no solution.
Otherwise, the solution :math:`k_{\text{sol}}` is:

.. math::
    :label: EqOrderStatAgen

    k_{\text{sol}} =
    \begin{cases}
    1 + \overline{F}_{(\sampleSize, \alpha)}^{-1}(\beta)
        & \text{if } \exists k_0 \mid \overline{F}_{(\sampleSize, \alpha)}(k_0 - 1) = \beta,\\
    \overline{F}_{(\sampleSize, \alpha)}^{-1}(\beta) & \text{otherwise}.
    \end{cases}

The following table presents the solution depending on the parameters.

+------------------------+----------------------+----------------------------+----------------------+
| :math:`k_{\text{sol}}` | :math:`\beta=0`      | :math:`0 < \beta < 1`      | :math:`\beta=1`      |
+========================+======================+============================+======================+
| :math:`\alpha=0`       | :math:`\sampleSize`  | :math:`\emptyset`          | :math:`\emptyset`    |
+------------------------+----------------------+----------------------------+----------------------+
| :math:`0 < \alpha < 1` | :math:`\sampleSize`  | see :eq:`EqOrderStatAgen`  | :math:`\emptyset`    |
+------------------------+----------------------+----------------------------+----------------------+
| :math:`\alpha=1`       | :math:`\sampleSize`  |  :math:`\sampleSize`       | :math:`\sampleSize`  |
+------------------------+----------------------+----------------------------+----------------------+

Ranks for bilateral bounds of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given the previously established data, we seek the ranks
:math:`k_1, k_2 \in \llbracket 1, \sampleSize \rrbracket^2`.
Because the optimal interval can be defined in various ways, the solution
depends on the chosen criterion.

Here are the different formulations based on the target objective.

**Criterion 1: Symmetric probability tails**

.. math::
    :label: EqOrderStatSymmetricProbability

    \begin{cases}
    \Prob{X_{(k_1)} \leq x_{\alpha}} \geq 1 - \dfrac{1 - \beta}{2}, \\
    \Prob{x_{\alpha} \leq X_{(k_2)}} \geq 1 - \dfrac{1 - \beta}{2}.
    \end{cases}

**Criterion 2: Minimizing the rank interval length**

.. math::
    :label: EqOrderStatMinRankLength

    \begin{aligned}
    (k_1, k_2) =
    & \argmin (k_2 - k_1) \\
    & \text{s.t. } \quad \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}}
      \geq \beta.
    \end{aligned}

**Criterion 3: Minimizing the probability of coverage**

.. math::
    :label: EqOrderStatMinCoverage

    \begin{aligned}
    (k_1, k_2) =
    & \argmin \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}} \\
    & \text{s.t. } \quad \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}}
      \geq \beta.
    \end{aligned}

**Criterion 4: Symmetric ranks**

Let :math:`k_\alpha` be the rank corresponding to the empirical estimator
of the quantile :math:`x_{\alpha}`:

.. math::

    k_\alpha
    = \begin{cases}
    \max(1, \alpha \sampleSize) & \textrm{ if } \alpha \sampleSize \textrm{ is an integer}, \\
    \min(\sampleSize, \lfloor\alpha \sampleSize\rfloor + 1) & \textrm{ otherwise.}
    \end{cases}

We search for the smallest :math:`\ell \leq \min(k_\alpha - 1, \sampleSize - k_\alpha + 1)`
such that:

.. math::
    :label: EqOrderStatSymmetricRanks

    \Prob{X_{(k_\alpha - \ell)} \leq x_{\alpha} \leq X_{(k_\alpha + \ell)}}
      \geq \beta.

The solutions to :eq:`EqOrderStatMinCoverage` and :eq:`EqOrderStatMinRankLength`
are determined numerically, using an optimization algorithm.

The solutions to :eq:`EqOrderStatSymmetricProbability` are such that:

.. math::

    \begin{aligned}
    F_{(\sampleSize, \alpha)}(k_1 - 1) \leq \dfrac{1 - \beta}{2}, \\
    F_{(\sampleSize, \alpha)}(k_2 - 1) \geq 1 - \dfrac{1 - \beta}{2}.
    \end{aligned}

The solutions are:

.. math::

    k_{1, \text{sol}}
    = F_{(\sampleSize, \alpha)}^{-1}\left(\dfrac{1 - \beta}{2}\right),

and:

.. math::

    k_{2, \text{sol}}
    = 1 + F_{(\sampleSize, \alpha)}^{-1}\left(1 - \dfrac{1 - \beta}{2}\right).

Minimum sample size for an upper bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given :math:`\alpha`, :math:`\beta`, and the rank
:math:`1 \leq k \leq \sampleSize`, we seek the smallest sample size
:math:`\sampleSize` such that:

.. math::

    \Prob{x_{\alpha} \leq X_{(\sampleSize - k + 1)}} \geq \beta.

The previous equation is equivalent to:

.. math::
    :label: EqOrderStat2Bbis

    F_{(\sampleSize, \alpha)}(\sampleSize - k)\geq \beta.

Note that the problem is defined differently from equation :eq:`EqOrderStatB`.
To determine this minimum sample size, we solve equation :eq:`EqOrderStat2Bbis`
with respect to the sample size :math:`\sampleSize`.
We use an optimization algorithm to compute :math:`\sampleSize_{\text{sol}}` in
the set :math:`\{ k, k + 1, \dots\}`.
We can reduce the search space to the interval
:math:`\llbracket k, \sampleSize_2 \rrbracket` where :math:`\sampleSize_2` is a
size that satisfies equation :eq:`EqOrderStat2Bbis`.
It can be determined using the approximation of the binomial distribution by the
normal distribution with the same mean and variance.

Once the smallest size :math:`\sampleSize` has been estimated, a sample of size
:math:`\sampleSize` can be generated from :math:`X` and an upper bound of
:math:`x_{\alpha}` is estimated using :math:`x_{(\sampleSize - k + 1)}`.

Minimum sample size for a lower bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given the same data as previously, we seek the smallest sample size
:math:`\sampleSize` such that equation :eq:`EqOrderStatA` is satisfied.

The solution is:

.. math::

    \sampleSize_{\text{sol}}
    = \min \left\{\sampleSize \geq k
       \mid \overline{F}_{(\sampleSize, \alpha)}(k - 1) \geq \beta \right\}.

The following table presents the solution depending on the parameters.

+------------------------------------+-----------------------------------+-----------------------------------+--------------------+
| :math:`\sampleSize_{\text{sol}}`   | :math:`\beta=0`                   | :math:`0 < \beta < 1`             | :math:`\beta=1`    |
+====================================+===================================+===================================+====================+
| :math:`\alpha=0`                   | :math:`k`                         | :math:`\emptyset`                 | :math:`\emptyset`  |
+------------------------------------+-----------------------------------+-----------------------------------+--------------------+
| :math:`0 < \alpha < 1`             | :math:`\sampleSize_{\text{sol}}`  | :math:`\sampleSize_{\text{sol}}`  | :math:`\emptyset`  |
+------------------------------------+-----------------------------------+-----------------------------------+--------------------+
| :math:`\alpha=1`                   | :math:`k`                         | :math:`k`                         | :math:`k`          |
+------------------------------------+-----------------------------------+-----------------------------------+--------------------+


Minimum sample size for bilateral bounds of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given two order statistics :math:`(k_1, k_2)` with
:math:`1 \leq k_1 < k_2 \leq \sampleSize`, we seek the smallest sample size
:math:`\sampleSize` such that:

.. math::
    :label: EqOrderMinSizeBil

    \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(\sampleSize - k_2 + 1)}} \geq \beta.

Equation :eq:`EqOrderMinSizeBil` can be rewritten as:

.. math::
    :label: EqOrderMinSizeBilsol

    F_{X_{(\sampleSize - k_2 + 1)}}(x_{\alpha})
    - F_{X_{(k_1)}}(x_{\alpha}) \geq \beta.

This can be equivalently expressed using the binomial distribution:

.. math::

    F_{(\sampleSize, \alpha)}(\sampleSize - k_2)
    - F_{(\sampleSize, \alpha)}(k_1 - 1) \geq \beta.

Note that the problem is defined differently from equation
:eq:`EqOrderStatMinRankLength`.
To determine this minimum sample size, we solve equation :eq:`EqOrderMinSizeBilsol`
with respect to the sample size :math:`\sampleSize`.
We use an optimization algorithm to compute :math:`\sampleSize_{\text{sol}}` in
the set :math:`\{ k, k + 1, \dots\}`.
We can reduce the search space to the interval
:math:`\llbracket k, \sampleSize_2 \rrbracket` where :math:`\sampleSize_2` is a
size that satisfies equation :eq:`EqOrderMinSizeBilsol`.
It can be determined using the approximation of the binomial distribution by the
normal distribution with the same mean and variance.

Once the smallest size :math:`\sampleSize` has been estimated, a sample of size
:math:`\sampleSize` can be generated from :math:`X` and a lower and an upper
bound of :math:`x_{\alpha}` are estimated using :math:`x_{(k_1)}` and
:math:`x_{(\sampleSize - k_2 + 1)}`.

In the particular case where :math:`(k_1, k_2) = (1,1)`, we seek the smallest
sample size :math:`\sampleSize` such that:

.. math::

    \Prob{ \min (X_1, \dots, X_\sampleSize)
    \leq x_{\alpha}
    \leq \max (X_1, \dots, X_\sampleSize)} \geq \beta.

Then, equation :eq:`EqOrderMinSizeBilsol` is equivalent to:

.. math::

    1 - \alpha^\sampleSize - (1 - \alpha)^\sampleSize \geq \beta.

The optimal :math:`\sampleSize` is determined using an optimization algorithm.
The search space is reduced to the interval:

.. math::

    \left \lfloor \dfrac{\log (1 - \beta)}{\log \gamma} \right \rfloor
    \leq \sampleSize
    \leq \left \lfloor \dfrac{\log \left(\dfrac{1 - \beta}{2}\right)}{\log \gamma} \right \rfloor,

where :math:`\gamma = \max(\alpha, 1 - \alpha)`.

.. topic:: API:

    - See :class:`~openturns.QuantileConfidence`
    - See :class:`~openturns.Sample`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/sample_analysis/plot_quantile_confidence_estimation`
    - See :doc:`/auto_data_analysis/sample_analysis/plot_quantile_confidence_chemical_process`
    - See :doc:`/auto_data_analysis/sample_analysis/plot_quantile_confidence_exact_empirical`

.. topic:: References:

    - [meeker2017]_
    - [wilks1948]_
    - [robert2004]_
    - [rubinstein2017]_
