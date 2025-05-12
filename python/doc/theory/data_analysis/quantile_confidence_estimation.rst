.. _quantile_confidence_estimation:

Estimation of a quantile bound
------------------------------

We consider a random variable :math:`X` of dimension 1 and the unknown  :math:`x_{\alpha}`
level quantile of its distribution (:math:`\alpha \in [0, 1]`).
We seek to evaluate an upper bound of :math:`x_{\alpha}` with a confidence greater or equal to
:math:`\beta`, using a given order statistics.

Let :math:`(X_1, \dots, X_\sampleSize)` be some independent copies of :math:`X`.
Let :math:`X_{(k)}` be the :math:`k` -th order statistics of :math:`(X_1, \dots, X_\sampleSize)` which means that
:math:`X_{(k)}` is the :math:`k` -th maximum of :math:`(X_1, \dots, X_\sampleSize)` for :math:`1 \leq k \leq \sampleSize`. For
example, :math:`X_{(1)} = \min (X_1, \dots, X_\sampleSize)` is the minimum
and :math:`X_{(\sampleSize)} = \max (X_1, \dots, X_\sampleSize)` is the maximum. We have:

.. math::

    X_{(1)} \leq X_{(2)} \leq \dots \leq X_{(\sampleSize)}



Rank for an upper bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`(x_1, \dots, x_\sampleSize)` be an i.i.d. sample of size :math:`\sampleSize` of
the random variable :math:`X`.
Given a quantile level :math:`\alpha \in [0,1]`, a confidence level
:math:`\beta \in [0,1]`, and a sample size :math:`\sampleSize`, we seek the smallest
rank :math:`k \in \llbracket 1, \sampleSize \rrbracket` such that:

.. math::
    :label: EqOrderStatB

    \Prob{x_{\alpha} \leq X_{(k)}} \geq \beta \qquad (B)

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
:math:`F_{(\sampleSize,F(x))}` is the cumulated
distribution function of the Binomial distribution :math:`\cB(\sampleSize,F(x))` and
:math:`\overline{F}_{(\sampleSize,F(x))}(k) = 1 - F_{(\sampleSize,F(x))}(k)` is the
complementary cumulated distribution fonction (also named survival function in dimension
1).
Therefore:

.. math::

    F_{X_{(k)}}(x_{\alpha}) = \sum_{i=k}^{\sampleSize} \binom{\sampleSize}{i} \alpha^i (1-\alpha)^{\sampleSize-i}
    = \overline{F}_{(\sampleSize,\alpha)}(k-1)

and equation :eq:`EqOrderStatB` implies:

.. math::
    :label: EqOrderStat2B

    1-F_{X_{(k)}}(x_{\alpha})\geq \beta

This implies:

.. math::

    F_{\sampleSize, \alpha}(k-1)\geq \beta

The smallest rank :math:`k_{sol}` such that the previous equation is satisfied is:

.. math::

    k_{sol} & = \min \{ k \in \llbracket 1, n \rrbracket \, | \, F_{\sampleSize, \alpha}(k-1)\geq \beta \}\\
            & = 1 +  \min \{ k \in \llbracket 1, n\rrbracket \, | \, F_{\sampleSize, \alpha}(k)\geq \beta \}

An upper bound of :math:`x_{\alpha}` is estimated by the value of :math:`X_{(k_{sol})}`
on the sample :math:`(x_1, \dots, x_\sampleSize)`.

Here is a recap of the existence of solutions for this case:

+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
|                        | :math:`\beta=0`                      | :math:`0 < \beta < 1`                   | :math:`\beta=1`                 |
+========================+======================================+=========================================+=================================+
| :math:`\alpha=0`       | :math:`1+F_{n,0}^{-1}(\beta)=1`      | :math:`1+F_{n,0}^{-1}(\beta)=1`         | :math:`1+F_{n,0}^{-1}(\beta)=1` |
+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
| :math:`0 < \alpha < 1` | :math:`1+F_{n,1}^{-1}(0)=1`          | 0 or :math:`1+F_{n,1}^{-1}(\beta)`      | :math:`\emptyset`               |
+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
| :math:`\alpha=1`       | 1                                    | :math:`\emptyset`                       | :math:`\emptyset`               |
+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+

Rank for a lower bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Similarly for the lower bound we seek the smallest rank :math:`k \in \llbracket 1, \sampleSize \rrbracket` such that:

.. math::
    :label: EqOrderStatA

    \Prob{X_{(k)} \leq x_{\alpha}} \geq \beta \qquad (A)

Here is a recap of the existence of solutions for this case:

+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
|                        | :math:`\beta=0`                      | :math:`0 < \beta < 1`                   | :math:`\beta=1`                 |
+========================+======================================+=========================================+=================================+
| :math:`\alpha=0`       | n                                    | :math:`\emptyset`                       | :math:`\emptyset`               |
+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
| :math:`0 < \alpha < 1` | :math:`F_{n,\alpha}^{-1}(1-\beta)=n` | see :eq:`EqOrderStatAgen`               | :math:`\emptyset`               |
+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
| :math:`\alpha=1`       | :math:`F_{n,1}^{-1}(1-\beta)=n`      | :math:`F_{n,1}^{-1}(1-\beta)=n`         | :math:`F_{n,1}^{-1}(1-\beta)=n` |
+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+

.. math::
    :label: EqOrderStatAgen

    \emptyset or F_{n,\alpha}^{-1}(1-\beta) or 1+F_{n,\alpha}^{-1}(1-\beta)

Ranks for bilateral bounds of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Similarly for the lower bound we seek the ranks :math:`k_1, k2 \in \llbracket 1, \sampleSize \rrbracket^2` such that:

.. math::
    :label: EqOrderStatC

    \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}} \geq \beta \qquad (C)

Here is a recap of the existence of solutions for this case:

+------------------------+-------------------------------------------------+---------------------------------+-------------------------+
|                        | :math:`\beta=0`                                 | :math:`0 < \beta < 1`           | :math:`\beta=1`         |
+========================+=================================================+=================================+=========================+
| :math:`\alpha=0`       | :math:`\Bigl\lfloor \frac{n}{2} \Bigr\rfloor`   | :math:`\emptyset`               | :math:`\emptyset`       |
+------------------------+-------------------------------------------------+---------------------------------+-------------------------+
| :math:`0 < \alpha < 1` | 1 solution                                      | :math:`\emptyset` or 1 solution | :math:`\emptyset`       |
+------------------------+-------------------------------------------------+---------------------------------+-------------------------+
| :math:`\alpha=1`       | :math:`\Bigl\lfloor \frac{n}{2} \Bigr\rfloor`   | :math:`\emptyset`               | :math:`\emptyset`       |
+------------------------+-------------------------------------------------+---------------------------------+-------------------------+

Minimum sample size for an upper bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given :math:`\alpha`, :math:`\beta`, and order :math:`k`, we seek for the smallest sample size
:math:`\sampleSize`
such that the equation :eq:`EqOrderStatB` is satisfied. In order to do so, we solve the
equation :eq:`EqOrderStat2B` with respect to the sample size :math:`\sampleSize`.

Once the smallest size :math:`\sampleSize` has been estimated, a sample of size :math:`\sampleSize` can be
generated from :math:`X` and an upper bound of :math:`x_{\alpha}` is estimated using
:math:`x_{(\sampleSize-k)}` i.e. the :math:`\sampleSize - k`-th observation
in the ordered sample :math:`(x_{(1)}, \dots, x_{(\sampleSize)})`.

Here is a recap of the existence of solutions for this case:

+--------------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
|                                | :math:`\beta=0`                      | :math:`0 < \beta < 1`                   | :math:`\beta=1`                 |
+================================+======================================+=========================================+=================================+
| :math:`\alpha=1`               | k                                    | 0                                                                         |
+--------------------------------+--------------------------------------+---------------------------------------------------------------------------+
| :math:`0 < \alpha < 1`         | :math:`k` if :math:`1-\alpha^k \geq \beta` else :math:`\emptyset`                                                |
+--------------------------------+------------------------------------------------------------------------------------------------------------------+
| :math:`\alpha=0`               | k                                                                                                                |
+--------------------------------+------------------------------------------------------------------------------------------------------------------+

Minimum sample size for a lower bound of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Similarly for the lower bound, we seek for the smallest sample size :math:`\sampleSize`
such that the equation :eq:`EqOrderStatB` is satisfied.

Here is a recap of the existence of solutions for this case:

+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
|                        | :math:`\beta=0`                      | :math:`0 < \beta < 1`                   | :math:`\beta=1`                 |
+========================+======================================+=========================================+=================================+
| :math:`\alpha=0`       | :math:`k`                            | :math:`\emptyset`                                                         |
+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
| :math:`0 < \alpha < 1` | :math:`\argmin{n \geq k} | f_{k,\alpha} \leq 1-\beta`                          | :math:`\emptyset`               |
+------------------------+--------------------------------------------------------------------------------+---------------------------------+
| :math:`\alpha=1`       | :math:`k`                                                                                                        |
+------------------------+------------------------------------------------------------------------------------------------------------------+


Minimum sample size for bilateral bounds of the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Similarly for the bilateral bounds, we seek for the smallest sample size :math:`\sampleSize`
such that the equation :eq:`EqOrderStatC` is satisfied.

Here is a recap of the existence of solutions for this case:

+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
|                        | :math:`\beta=0`                      | :math:`0 < \beta < 1`                   | :math:`\beta=1`                 |
+========================+======================================+=========================================+=================================+
| :math:`\alpha=1`       | :math:`k2`                           | :math:`\emptyset`                                                         |
+------------------------+--------------------------------------+-----------------------------------------+---------------------------------+
| :math:`0 < \alpha < 1` | :math:`k_2` if :math:`1-\alpha^{k_2} - F_{k_2,\alpha}(k_1-1) \geq \beta` else :math:`\emptyset`                  |
+------------------------+--------------------------------------------------------------------------------+---------------------------------+
| :math:`\alpha=0`       | :math:`\emptyset` if :math:`k1 \neq 0` and :math:`\beta > 0` else :math:`k_2`                                    |
+------------------------+------------------------------------------------------------------------------------------------------------------+


.. topic:: API:

    - See :class:`~openturns.experimental.QuantileConfidence`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/manage_data_and_samples/plot_quantile_confidence_estimation`
    - See :doc:`/auto_data_analysis/manage_data_and_samples/plot_quantile_confidence_chemical_process`

.. topic:: References:

    - [meeker2017]_
    - Wilks, S. S. (1941). Determination of sample sizes for setting tolerance limits. The Annals of Mathematical Statistics, 12(1), 91-96
    - Robert C.P., Casella G. (2004). Monte-Carlo Statistical Methods, Springer, ISBN 0-387-21239-6, 2nd ed.
    - Rubinstein R.Y. (1981). Simulation and The Monte-Carlo methods, John Wiley & Sons
