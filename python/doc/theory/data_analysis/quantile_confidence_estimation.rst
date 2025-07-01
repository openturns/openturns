.. _quantile_confidence_estimation:

Exact quantile confidence interval based on order statistics
------------------------------------------------------------

We consider a random variable :math:`X` of dimension 1 and its quantile :math:`x_{\alpha}`
of level :math:`\alpha` (:math:`\alpha \in [0, 1]`).
We seek to evaluate an upper bound of :math:`x_{\alpha}` with a confidence greater or equal to
:math:`\beta`, using order statistics.

Let :math:`(X_1, \dots, X_\sampleSize)` be some independent copies of :math:`X`.
Let :math:`X_{(k)}` be the :math:`k` -th order statistics of :math:`(X_1, \dots, X_\sampleSize)` which means that
:math:`X_{(k)}` is the :math:`k` -th minimum of :math:`(X_1, \dots, X_\sampleSize)` for :math:`1 \leq k \leq \sampleSize`. For
example, :math:`X_{(1)} = \min (X_1, \dots, X_\sampleSize)` is the minimum
and :math:`X_{(\sampleSize)} = \max (X_1, \dots, X_\sampleSize)` is the maximum. We have:

.. math::

    X_{(1)} \leq X_{(2)} \leq \dots \leq X_{(\sampleSize)}

The probability density and cumulative distribution functions of the order
statistics :math:`X_{(k)}` are:

.. math::
    :label: DistOrderStat

    F_{X_{(k)}}(x) & = \sum_{i=k}^{\sampleSize} \binom{\sampleSize}{i}\left(F(x)
    \right)^i \left(1-F(x)
    \right)^{\sampleSize-i} \\
    p_{X_{(k)}}(x) & = (\sampleSize-k+1)\binom{\sampleSize}{k-1}\left(F(x)\right)^{k-1}
    \left(1-F(x)
    \right)^{\sampleSize-k} p(x)

We notice that :math:`F_{X_{(k)}}(x) = \overline{F}_{(\sampleSize,F(x))}(k-1)` where
:math:`F_{(\sampleSize,F(x))}` is the cumulative
distribution function of the Binomial distribution :math:`\cB(\sampleSize,F(x))` and
:math:`\overline{F}_{(\sampleSize,F(x))}(k) = 1 - F_{(\sampleSize,F(x))}(k)` is the
complementary cumulative distribution fonction (also named survival function in dimension
1).
Therefore:

.. math::

    F_{X_{(k)}}(x_{\alpha}) = \sum_{i=k}^{\sampleSize} \binom{\sampleSize}{i} \alpha^i (1-\alpha)^{\sampleSize-i}
    = \overline{F}_{(\sampleSize,\alpha)}(k-1)

Rank for an upper bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`(x_1, \dots, x_\sampleSize)` be an i.i.d. sample of size :math:`\sampleSize` of
the random variable :math:`X`.
Given a quantile level :math:`\alpha \in [0,1]`, a confidence level
:math:`\beta \in [0,1]`, and a sample size :math:`\sampleSize`, we seek the smallest
rank :math:`k \in \llbracket 1, \sampleSize \rrbracket` such that:

.. math::
    :label: EqOrderStatB

    \Prob{x_{\alpha} \leq X_{(k)}} \geq \beta

As equation :eq:`EqOrderStatB` can be written as:

.. math::
    :label: EqOrderStat2B

    1-F_{X_{(k)}}(x_{\alpha})\geq \beta

or even as:

.. math::

    F_{\sampleSize, \alpha}(k-1)\geq \beta

Then, the smallest rank :math:`k_{sol}` such that the previous equation is satisfied is:

.. math::

    k_{sol} & = \min \{ k \in \llbracket 1, \sampleSize \rrbracket \, | \, F_{\sampleSize, \alpha}(k-1)\geq \beta \}\\
            & = 1 + \min \{ k \in \llbracket 1, \sampleSize\rrbracket \, | \, F_{\sampleSize, \alpha}(k)\geq \beta \}

An upper bound of :math:`x_{\alpha}` is estimated by the value of :math:`X_{(k_{sol})}`
on the sample :math:`(x_1, \dots, x_\sampleSize)`.

Here is a recap of the existence of solutions for this case:

+------------------------+------------------+-------------------------------------+---------------------------------+
| :math:`k_{sol}`        | :math:`\beta=0`  | :math:`0 < \beta < 1`               | :math:`\beta=1`                 |
+========================+==================+=====================================+=================================+
| :math:`\alpha=0`       | 1                | 1                                   | 1                               |
+------------------------+------------------+-------------------------------------+---------------------------------+
| :math:`0 < \alpha < 1` | 1                | see :eq:`EqOrderStatBgen`           | :math:`\emptyset`               |
+------------------------+------------------+-------------------------------------+---------------------------------+
| :math:`\alpha=1`       | 1                | :math:`\emptyset`                   | :math:`\emptyset`               |
+------------------------+------------------+-------------------------------------+---------------------------------+

where:

.. math::
    :label: EqOrderStatBgen

    k_{sol} & = 1 + F_{\sampleSize,\alpha}^{-1}(\beta)   \quad \text{if} \quad  1-\alpha^\sampleSize \geq \beta \\
            & = \emptyset   \quad \text{else}


Rank for a lower bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given the same data as previoulsy, we seek the greatest rank :math:`k \in \llbracket 1, \sampleSize \rrbracket` such that:

.. math::
    :label: EqOrderStatA

    \Prob{X_{(k)} \leq x_{\alpha}} \geq \beta

which can be written as:

.. math::
    :label: EqOrderStat2A

    F_{X_{(k)}}(x_{\alpha})\geq \beta

and finally as:

.. math::

    F_{\sampleSize, \alpha}(k - 1)\leq 1 - \beta

Then, the greatest rank :math:`k_{sol}` such that the previous equation is satisfied is:


.. math::

    k_{sol} & = \max \{ k \in \llbracket 1, \sampleSize \rrbracket \, | \, F_{\sampleSize, \alpha}(k-1)\leq \beta \}\\
            & = 1+\max \{ k \in \llbracket 1, \sampleSize\rrbracket \, | \, F_{\sampleSize, \alpha}(k)\leq \beta \}

Here is a recap of the existence of solutions for this case:

+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
| :math:`k_{sol}`        | :math:`\beta=0`                      | :math:`0 < \beta < 1`                   | :math:`\beta=1`                 |
+========================+======================================+=========================================+=================================+
| :math:`\alpha=0`       | :math:`\sampleSize`                  | :math:`\emptyset`                       | :math:`\emptyset`               |
+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
| :math:`0 < \alpha < 1` | :math:`\sampleSize`                  | see :eq:`EqOrderStatAgen`               | :math:`\emptyset`               |
+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
| :math:`\alpha=1`       | :math:`\sampleSize`                  |  :math:`\sampleSize`                    | :math:`\sampleSize`             |
+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+

where:

.. math::
    :label: EqOrderStatAgen

    k_{sol} & = \emptyset \quad \text{if} \quad  (1-\alpha)^\sampleSize > 1 - \beta \\
            & = 1 + F_{\sampleSize,\alpha}^{-1}(1-\beta)  \quad  \text{otherwise and if} \quad  \exists k_0 \, | \, 1-\beta = F_{(\sampleSize,\alpha}(k_0 - 1) \\
            & = F_{\sampleSize,\alpha}^{-1}(1-\beta)  \quad  \text{else}


Ranks for bilateral bounds of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given the same data as previoulsy, we can seek the ranks
:math:`k_1, k_2 \in \llbracket 1, \sampleSize \rrbracket^2` as solution of different problems.

The problem can be:

.. math::
    :label: EqOrderStatC3

    \begin{array}{ll}
    (k_1, k_2) = & \argmin \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}}\\
                 & \mbox{s.t.} \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}} \geq \beta
    \end{array}


or:

.. math::
    :label: EqOrderStatC4

    \begin{array}{ll}
    (k_1, k_2) = & \argmin (k_2-k_1)\\
                 & \mbox{s.t.} \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}} \geq \beta
    \end{array}


or:

.. math::
    :label: EqOrderStatC1

    \Prob{X_{(k_1)} \leq x_{\alpha} } \geq 1 - \dfrac{1-\beta}{2}\\
    \Prob{x_{\alpha} \leq X_{(k_2)}} \geq 1 - \dfrac{1-\beta}{2}


or with :math:`(k_1, k_2) = (k,\sampleSize-k+1)` and :math:`1 \leq k \leq \sampleSize` the greatest rank such that:

.. math::
    :label: EqOrderStatC2

    \Prob{X_{(k)} \leq x_{\alpha} \leq X_{(\sampleSize-k+1)}} \geq \beta.


The solutions of :eq:`EqOrderStatC3` and :eq:`EqOrderStatC4` are determined numerically, using an optimization algorithm.

The solutions of :eq:`EqOrderStatC1` are respectively defined by:

.. math::

   \overline{F}_{(\sampleSize,\alpha)}(k_1 - 1) \leq \dfrac{1 - \beta}{2} \\
   F_{(\sampleSize,\alpha)}(k_2 - 1) \geq 1-\dfrac{1 - \beta}{2}

which leads to the respective solutions:

.. math::

    k_{1, sol} & = \max \{ k \in \llbracket 1, \sampleSize \rrbracket \, | \, F_{\sampleSize, \alpha}(k-1)\leq \dfrac{1 - \beta}{2} \}\\
            & = 1 + \max \{ k \in \llbracket 1, \sampleSize\rrbracket \, | \, F_{\sampleSize, \alpha}(k)\leq \dfrac{1-\beta}{2} \}

and

.. math::

    k_{2, sol} & = \min \{ k \in \llbracket 1, \sampleSize \rrbracket \, | \, F_{\sampleSize, \alpha}(k - 1)\geq 1- \dfrac{1 - \beta}{2} \}\\
            & = 1 + \min \{ k \in \llbracket 1, \sampleSize\rrbracket \, | \, F_{\sampleSize, \alpha}(k)\geq 1 - \dfrac{1 - \beta}{2} \}

Then, the previous tables written for the lower and upper bounds can be used to find :math:`k_{1, sol}` and :math:`k_{2, sol}` respectively with
:math:`\beta \rightarrow \dfrac{1-\beta}{2}` or :math:`\beta \rightarrow 1-\dfrac{1-\beta}{2}`.

The solutions of :eq:`EqOrderStatC2` are gathered here:

+------------------------+-----------------------------------------------------------+---------------------------------+-------------------------+
| :math:`k_{sol}`        | :math:`\beta=0`                                           | :math:`0 < \beta < 1`           | :math:`\beta=1`         |
+========================+===========================================================+=================================+=========================+
| :math:`\alpha=0`       | :math:`\Bigl\lfloor \frac{n}{2} \Bigr\rfloor`             | :math:`\emptyset`               | :math:`\emptyset`       |
+------------------------+-----------------------------------------------------------+---------------------------------+-------------------------+
| :math:`0 < \alpha < 1` | 1                                                         | :math:`\emptyset` or 1          | :math:`\emptyset`       |
+------------------------+-----------------------------------------------------------+---------------------------------+-------------------------+
| :math:`\alpha=1`       | :math:`\Bigl\lfloor \frac{\sampleSize}{2} \Bigr\rfloor`   | :math:`\emptyset`               | :math:`\emptyset`       |
+------------------------+-------------------------------------------------+---------------------------------+-----------------------------------+

Minimum sample size for an upper bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given :math:`\alpha`, :math:`\beta`, and the rank :math:`1 \leq k \leq \sampleSize`, we seek the smallest sample size
:math:`\sampleSize` such that:

.. math::
    :label: EqOrderStatBbis

    \Prob{x_{\alpha} \leq X_{(\sampleSize-k+1)}} \geq \beta

As equation :eq:`EqOrderStatBbis` can be written as:

.. math::
    :label: EqOrderStat2Bbis

    1-F_{X_{(\sampleSize-k+1)}}(x_{\alpha})\geq \beta

or even as:

.. math::

    F_{\sampleSize, \alpha}(\sampleSize-k)\geq \beta

Note that the problem is defined differently than in equation :eq:`EqOrderStatB`. In order to do so, we solve
equation :eq:`EqOrderStat2Bbis` with respect to the sample size :math:`\sampleSize`. We use an optimization algorithm to determined
:math:`n_{sol}` in the interval :math:`\llbracket k, +\infty \llbracket`. We can reduce the research interval to the interval
:math:`\llbracket k, n_2 \rrbracket` where :math:`n_2` is a size
that verifies equation :eq:`EqOrderStat2Bbis`. It
can be determined using the approximation of the binomial distribution by the normal distribution with the same mean and variance.

Once the smallest size :math:`\sampleSize` has been estimated, a sample of size :math:`\sampleSize` can be
generated from :math:`X` and an upper bound of :math:`x_{\alpha}` is estimated using
:math:`x_{(n-k+1)}` i.e. the :math:`k`-th observation
in the decreasing ordered sample :math:`(x_{(\sampleSize)}, \dots, x_{(1)})`.

Minimum sample size for a lower bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given the same data as previoulsy, we seek the smallest sample size :math:`\sampleSize`
such that equation :eq:`EqOrderStatA` is satisfied.

Here is a recap of the existence of solutions for this case:

+------------------------+--------------------------------------+---------------------------------------------+---------------------------------+
|       :math:`n_{sol}`  | :math:`\beta=0`                      | :math:`0 < \beta < 1`                       | :math:`\beta=1`                 |
+========================+======================================+=============================================+=================================+
| :math:`\alpha=0`       | :math:`k`                            | :math:`\emptyset`                           | :math:`\emptyset`               |
+------------------------+--------------------------------------+---------------------------------------------+---------------------------------+
| :math:`0 < \alpha < 1` | :math:`\argmin \{\sampleSize \geq k | F_{\sampleSize,\alpha}(k-1) \leq 1-\beta \}` | :math:`\emptyset`               |
+------------------------+--------------------------------------+---------------------------------------------+---------------------------------+
| :math:`\alpha=1`       | :math:`k`                            | :math:`k`                                   | :math:`k`                       |
+------------------------+--------------------------------------+---------------------------------------------+---------------------------------+


Minimum sample size for bilateral bounds of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given two order statistics :math:`(k_1, k_2)` with :math:`1 \leq k_1 < k_2 \leq \sampleSize`, we seek the smallest sample size :math:`\sampleSize`
such that:

.. math::
    :label: EqOrderMinSizeBil

    \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(n-k_2+1)}} \geq \beta

As equation :eq:`EqOrderMinSizeBil` can be written as:

.. math::
    :label: EqOrderMinSizeBilsol

    F_{X_{(\sampleSize-k_2+1)}}(x_{\alpha}) - F_{X_{(k_1)}}(x_{\alpha}) \geq \beta

or even as:

.. math::

    F_{\sampleSize, \alpha}(\sampleSize-k_2) - F_{\sampleSize, \alpha}(k_1-1)\geq \beta


Note that the problem is defined differently than in equation :eq:`EqOrderStatC4`. In order to do so, we solve
equation :eq:`EqOrderMinSizeBilsol` with respect to the sample size :math:`\sampleSize`. We use an optimization algorithm to determined
:math:`n_{sol}` in the interval :math:`\llbracket k, +\infty \llbracket`. We can reduce the research interval to the interval
:math:`\llbracket k, n_2 \rrbracket` where :math:`n_2` is a size
that verifies equation :eq:`EqOrderStat2Bbis`. It
can be determined using the approximation of the binomial distribution by the normal distribution with the same mean and variance.

Once the smallest size :math:`\sampleSize` has been estimated, a sample of size :math:`\sampleSize` can be
generated from :math:`X` and an lower and upper bound of :math:`x_{\alpha}` is estimated using
:math:`x_{(k_1)}` and :math:`x_{(n-k_2+1)}` i.e. the :math:`k_1`-th observation
in the ordered sample :math:`(x_{(1)}, \dots, x_{(\sampleSize)})` and the :math:`\sampleSize-k_2`-th observation
in the decreasing ordered sample :math:`(x_{(\sampleSize)}, \dots, x_{(1)})`.

In the particular case where :math:`(k_1, k_2) = (1,1)`, we seek the smallest sample size :math:`\sampleSize`
such that:

.. math::

    \Prob{ \min (X_1, \dots, X_\sampleSize) \leq x_{\alpha} \leq  \max (X_1, \dots, X_\sampleSize)} \geq \beta.

Then, equantion :eq:`EqOrderMinSizeBilsol` can be written as:

.. math::

    1-\alpha^\sampleSize - (1-\alpha)^\sampleSize \geq \beta.

The optimal :math:`\sampleSize` is determined using an optimization algorithm which research is reduced to the interval:

.. math::

    \left \lfloor \dfrac{\log (1-\beta)}{\log \gamma} \right \rfloor \leq n \leq \left \lfloor \dfrac{\log \left(\dfrac{1-\beta}{2}\right)}{\log \gamma} \right \rfloor

where :math:`\gamma = \max(\alpha, 1-\alpha)`.

.. topic:: API:

    - See :class:`~openturns.experimental.QuantileConfidence`
    - See :class:`~openturns.Sample`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/manage_data_and_samples/plot_quantile_confidence_estimation`
    - See :doc:`/auto_data_analysis/manage_data_and_samples/plot_quantile_confidence_chemical_process`
    - See :doc:`/auto_data_analysis/manage_data_and_samples/plot_quantile_confidence_exact_empirical`

.. topic:: References:

    - [meeker2017]_
    - [wilks1941]_
    - [robert2004]_
    - [rubinstein2017]_
