.. _quantile_asymptotic_confidence_estimation:

Asymptotic quantile confidence interval based on order statistics
-----------------------------------------------------------------

We consider a random variable :math:`X` of dimension 1 and its quantile :math:`x_{\alpha}`
of level :math:`\alpha` (:math:`\alpha \in [0, 1]`).
We want to determine an asymptotic confidence interval of :math:`x_{\alpha}` with a confidence greater or equal to
:math:`\beta`, using order statistics.

Let :math:`(X_1, \dots, X_\sampleSize)` be some independent copies of :math:`X`.

Let :math:`(X_1, \dots, X_\sampleSize)` be some independent copies of :math:`X`.
Let :math:`X_{(k)}` be the :math:`k` -th order statistics of :math:`(X_1, \dots, X_\sampleSize)` which means that
:math:`X_{(k)}` is the :math:`k` -th minimum of :math:`(X_1, \dots, X_\sampleSize)` for :math:`1 \leq k \leq \sampleSize`. For
example, :math:`X_{(1)} = \min (X_1, \dots, X_\sampleSize)` is the minimum
and :math:`X_{(\sampleSize)} = \max (X_1, \dots, X_\sampleSize)` is the maximum.

Empirical quantile estimator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We first introduce the empirical estimator of the quantile :math:`x_{\alpha}`.
We denote by :math:`\hat{F}` the empirical cumulative distribution function defined by:

 .. math::

        \hat{F}(x) = \dfrac{1}{\sampleSize} \sum_{i=1}^\sampleSize 1_{X_i \leq x}

Then, the empirical estimator :math:`x_{\alpha}` is defined by:

 .. math::

        \hat{X}_{\alpha} = \inf \{ x, \hat{F}(x) \geq \alpha \} = X_{(\lceil \sampleSize\alpha \rceil)}

where :math:`\lceil x \rceil` is the smallest integer value that is greater than or equal to :math:`x`.

The empirical estimator is asymptotically normal (see [delmas2006]_, [garnier2008]_):

 .. math::

        \lim_{\sampleSize \to +\infty} \sqrt{\sampleSize}( \hat{X}_{\alpha}) - x_{\alpha}) = \cN(0, \sigma^2)
        \quad \mbox{with}  \quad \sigma^2 = \dfrac{\alpha(1-\alpha)}{p^2(x_{\alpha})}

The empirical estimator has a bias and a variance of order :math:`1/\sampleSize` (see [david1981]_, [garnier2008]_, [Motoyama2025]_). We get
the following asymptotic results:

 .. math::

        \Expect{\hat{X}_{\alpha}} & = x_{\alpha} - \dfrac{\alpha(1-\alpha)p'(x_{\alpha})}{2(\sampleSize+2)p^3(x_{\alpha})} + O\left(
        \dfrac{1}{\sampleSize^2}\right)\\
        \Var{\hat{X}_{\alpha}} & = \dfrac{\alpha(1-\alpha)}{(\sampleSize+2)p^2(x_{\alpha})} + O\left(\dfrac{1}{\sampleSize^2}\right)

where :math:`p` is the (continuously differentiable) density of :math:`X`. This result is not very useful for the construction of a
confidence interval as :math:`p(x_{\alpha})` is not known!


Asymptotic quantile confidence interval
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Here, we seek an asymptotic confidence interval of level :math:`\beta` based on order statistics. This confidence interval is
:math:`\left[ X_{(i_\sampleSize)}, X_{(j_\sampleSize)}\right]` where the ranks :math:`i_\sampleSize` and :math:`j_\sampleSize` are
defined by:

 .. math::

     i_\sampleSize & = \left\lfloor \sampleSize \alpha - \sqrt{\sampleSize} \; z_{\frac{1+\beta}{2}} \; \sqrt{\alpha(1 - \alpha)} \right\rfloor\\
     j_\sampleSize & = \left\lfloor \sampleSize \alpha + \sqrt{\sampleSize} \; z_{\frac{1+\beta}{2}} \;  \sqrt{\alpha(1 - \alpha)} \right\rfloor

where  :math:`z_{\frac{1+\beta}{2}}` is the :math:`\frac{1+\beta}{2}` level quantile of the standard normal distribution (see [delmas2006]_
proposition 11.1.13).

Then, we have:

 .. math::

     \lim\limits_{\sampleSize \rightarrow +\infty} \Prob{x_{\alpha} \in \left[ X_{(i_\sampleSize,\sampleSize)}, X_{(j_\sampleSize,
     \sampleSize)}\right]} = \beta


.. topic:: API:

    - See :class:`~openturns.experimental.QuantileConfidence`
    - See :class:`~openturns.Sample`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/sample_analysis/plot_quantile_confidence_estimation`
    - See :doc:`/auto_data_analysis/sample_analysis/plot_quantile_confidence_chemical_process`
    - See :doc:`/auto_data_analysis/sample_analysis/plot_quantile_confidence_exact_empirical`

.. topic:: References:

    - [meeker2017]_
    - [wilks1941]_
    - [robert2004]_
    - [rubinstein2017]_
    - [garnier2008]_
    - [david1981]_
    - [Motoyama2025]_
    - [delmas2006]_
