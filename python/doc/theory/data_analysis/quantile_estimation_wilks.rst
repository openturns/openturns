.. _quantile_estimation_wilks:

Estimation of a quantile upper bound by Wilks' method
-----------------------------------------------------

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


Smallest rank for an upper bound to the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`(x_1, \dots, x_\sampleSize)` be an i.i.d. sample of size :math:`\sampleSize` of
the random variable :math:`X`.
Given a quantile level :math:`\alpha \in [0,1]`, a confidence level
:math:`\beta \in [0,1]`, and a sample size :math:`\sampleSize`, we seek the smallest
rank :math:`k \in \llbracket 1, \sampleSize \rrbracket` such that:

.. math::
    :label: EqOrderStat

    \Prob{x_{\alpha} \leq X_{(k)}} \geq \beta

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

and equation :eq:`EqOrderStat` implies:

.. math::
    :label: EqOrderStat2

    1-F_{X_{(k)}}(x_{\alpha})\geq \beta

This implies:

.. math::

    F_{\sampleSize, \alpha}(k-1)\geq \beta

The smallest rank :math:`k_{sol}` such that the previous equation is satisfied is:

.. math::

    k_{sol} & = \min \{ k \in \llbracket 1, n \rrbracket \, | \, F_{\sampleSize, \alpha}(k-1)\geq \beta \}\\
            & = 1 +  \min \{ k \in \llbracket 1, n\rrbracket \, | \, F_{\sampleSize, \alpha}(k)\geq \beta \}

An upper bound of  :math:`x_{\alpha}` is estimated by the value of :math:`X_{(k_{sol})}`
on the sample
:math:`(x_1, \dots, x_\sampleSize)`.

Minimum sample size for an upper bound to the quantile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given :math:`\alpha`, :math:`\beta`, and :math:`k`, we seek for the smallest sample size
:math:`\sampleSize`
such that the equation :eq:`EqOrderStat` is satisfied. In order to do so, we solve the
equation :eq:`EqOrderStat2` with respect to the sample size :math:`\sampleSize`.

Once the smallest size :math:`\sampleSize`  has been estimated, a sample of size
:math:`\sampleSize` can be
generated from
:math:`X` and an upper bound of :math:`x_{\alpha}` is estimated using
:math:`x_{(\sampleSize-i)}` i.e. the :math:`\sampleSize - i`-th observation
in the ordered sample :math:`(x_{(1)}, \dots, x_{(\sampleSize)})`.


.. topic:: API:

    - See :class:`~openturns.Wilks`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/manage_data_and_samples/plot_quantile_estimation_wilks`

.. topic:: References:

    - Wilks, S. S. (1941). Determination of sample sizes for setting tolerance limits. The Annals of Mathematical Statistics, 12(1), 91-96
    - Robert C.P., Casella G. (2004). Monte-Carlo Statistical Methods, Springer, ISBN 0-387-21239-6, 2nd ed.
    - Rubinstein R.Y. (1981). Simulation and The Monte-Carlo methods, John Wiley & Sons
