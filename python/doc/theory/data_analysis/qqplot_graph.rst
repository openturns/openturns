.. _qqplot_graph:

QQ-plot
-------

The Quantile - Quantile - Plot (QQ Plot) enables to validate whether
two given samples of data
are drawn from the same continuous distribution of dimension 1.

We denote by :math:`\left\{ x_1,\ldots,x_{\sampleSize} \right\}`
and :math:`\left\{ y_1,\ldots,y_{\sampleSize} \right\}` two given samples of dimension 1.

A QQ-Plot is based on the comparison of some empirical quantiles. Let :math:`q_{X}(\alpha)`
be the quantile of order
:math:`\alpha` of the distribution :math:`F`, with :math:`\alpha \in (0, 1)`. It is defined by:

.. math::

   \begin{aligned}
       q_{X}(\alpha) = \inf \{ x \in \Rset \, |\, F(x) \geq \alpha \}
     \end{aligned}

The empirical quantile of order :math:`\alpha` built on the sample is defined by:

.. math::

    \begin{aligned}
            \widehat{q}_{X}(\alpha) = x_{([\sampleSize \alpha]+1)}
    \end{aligned}

where :math:`[\sampleSize\alpha]` denotes the integral part of :math:`\sampleSize \alpha`
and :math:`\left\{ x_{(1)},\ldots,x_{(\sampleSize)} \right\}` is the sample sorted in ascended order:

.. math::

    x_{(1)} \leq \dots \leq x_{(\sampleSize)}

Thus, the :math:`j^\textrm{th}` smallest value of the sample
:math:`x_{(j)}` is an estimate :math:`\widehat{q}_{X}(\alpha)` of the
:math:`\alpha`-quantile where :math:`\alpha = (j-1)/\sampleSize`, for :math:`1 < j \leq \sampleSize`.

The QQ-plot draws the couples of empirical quantiles of the same order from both samples:
:math:`(x_{(j)}, y_{(j)})_{1 < j \leq \sampleSize}`.
If both samples follow the same distribution, then the points should be close to the diagonal.

The following figure illustrates a QQ-plot with two
samples of size :math:`\sampleSize=50`. In this example, the
points remain close to the diagonal and the hypothesis
“Both samples are drawn from the same distribution” does not seem false,
even if a more quantitative analysis should be
carried out to confirm this.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    distribution = ot.Normal(3.0, 2.0)
    sample = distribution.getSample(150)
    graph = ot.VisualTest.DrawQQplot(sample, distribution)
    View(graph)

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    distribution = ot.Normal(3.0, 3.0)
    distribution2 = ot.Normal(2.0, 1.0)
    sample = distribution.getSample(150)
    graph = ot.VisualTest.DrawQQplot(sample, distribution2)
    View(graph)

In this second example, the two samples clearly arise from two different
distributions.


.. topic:: API:

    - See :py:func:`~openturns.VisualTest.DrawQQplot` to draw a QQ plot

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/statistical_tests/plot_qqplot_graph`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
    - [nisthandbook]_
    - [dagostino1986]_
    - [bhattacharyya1997]_
    - [sprent2001]_
