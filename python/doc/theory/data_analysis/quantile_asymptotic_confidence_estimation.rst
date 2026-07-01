.. _quantile_asymptotic_confidence_estimation:

Asymptotic quantile confidence interval based on order statistics
-----------------------------------------------------------------

We consider a one-dimensional random variable :math:`X` and its quantile
:math:`x_{\alpha}` of level :math:`\alpha`, where :math:`\alpha \in (0, 1)`.
We want to determine an asymptotic confidence interval for :math:`x_{\alpha}`
with a confidence level greater than or equal to :math:`\beta`, using order
statistics.

Let :math:`(X_1, \dots, X_\sampleSize)` be independent copies of :math:`X`.
Let :math:`X_{(k)}` be the :math:`k`-th order statistic of
:math:`(X_1, \dots, X_\sampleSize)`:

.. math::
    X_{(1)} \leq X_{(2)} \leq \cdots \leq X_{(\sampleSize)}.

Empirical quantile estimator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We first introduce the empirical estimator of the quantile :math:`x_{\alpha}`.
We denote by :math:`\widehat{F}` the empirical cumulative distribution function
defined by:

.. math::

    \widehat{F}(x)
    = \dfrac{1}{\sampleSize} \sum_{i=1}^\sampleSize \mathbf{1}_{X_i \leq x}

where :math:`\mathbf{1}_{X_i \leq x}` is the indicator function of
:math:`X_i \leq x`.
Then, the empirical estimator :math:`\widehat{X}_{\alpha}` is defined by:

.. math::

    \begin{aligned}
    \widehat{X}_{\alpha}
    & = \inf \left\{ x \mid \widehat{F}(x) \geq \alpha \right\} \\
    & = X_{(\lceil \sampleSize\alpha \rceil)}
    \end{aligned}

where :math:`\lceil x \rceil` is the smallest integer value that is greater than or equal to :math:`x`.

The empirical estimator is asymptotically normal (see [delmas2006]_, [garnier2008]_):

.. math::

    \sqrt{\sampleSize} \left( \widehat{X}_{\alpha} - x_{\alpha}\right)
    \xrightarrow{\mathcal{L}} \cN(0, \sigma^2)

with:

.. math::

    \sigma^2 = \dfrac{\alpha (1 - \alpha)}{p^2(x_{\alpha})}.

where :math:`p` is the (continuously differentiable) density of :math:`X`.
The empirical estimator has a bias and a variance of order :math:`1/\sampleSize`
(see [david1981]_, [garnier2008]_, [Motoyama2025]_).
We get the following asymptotic results:

.. math::

    \begin{aligned}
    \Expect{\widehat{X}_{\alpha}} &
    = x_{\alpha} - \dfrac{\alpha(1-\alpha)p'(x_{\alpha})}{2(\sampleSize+2)p^3(x_{\alpha})}
      + O\left(\dfrac{1}{\sampleSize^2}\right)\\
    \Var{\widehat{X}_{\alpha}}
    & = \dfrac{\alpha(1-\alpha)}{(\sampleSize+2)p^2(x_{\alpha})}
        + O\left(\dfrac{1}{\sampleSize^2}\right).
    \end{aligned}

These results are not very useful for the construction of a
confidence interval as :math:`p(x_{\alpha})` is not known.

Asymptotic quantile confidence interval
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Here, we seek an asymptotic confidence interval of level :math:`\beta` based on
order statistics.
This confidence interval is
:math:`\left[ X_{(i_\sampleSize)}, X_{(j_\sampleSize)}\right]` where the ranks
:math:`i_\sampleSize` and :math:`j_\sampleSize` are defined by:

.. math::

    \begin{aligned}
    i_\sampleSize & = \left\lfloor \sampleSize \alpha - \sqrt{\sampleSize} \; z_{\frac{1+\beta}{2}} \; \sqrt{\alpha(1 - \alpha)} \right\rfloor\\
    j_\sampleSize & = \left\lfloor \sampleSize \alpha + \sqrt{\sampleSize} \; z_{\frac{1+\beta}{2}} \;  \sqrt{\alpha(1 - \alpha)} \right\rfloor
    \end{aligned}

where :math:`z_{\frac{1+\beta}{2}}` is the quantile of level :math:`\frac{1+\beta}{2}`
of the standard normal distribution (see [delmas2006]_, Proposition 11.1.13).

Then, we have:

.. math::

    \lim\limits_{\sampleSize \rightarrow +\infty}
    \Prob{x_{\alpha} \in \left[ X_{(i_\sampleSize)}, X_{(j_\sampleSize)}\right]}
    = \beta.

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
    - [garnier2008]_
    - [david1981]_
    - [Motoyama2025]_
    - [delmas2006]_
