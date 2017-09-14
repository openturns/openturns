Using QQ-plot to compare two samples
------------------------------------

Let :math:`X` be a scalar uncertain variable modeled as a random
variable. This method deals with the construction of a dataset prior to
the choice of a probability distribution for :math:`X`. A QQ-plot (where
“QQ” stands for “quantile-quantile”) is a tool that may be used to
compare two samples :math:`\left\{x_1,\ldots,x_N \right\}` and
:math:`\left\{x'_1,\ldots,x'_M \right\}`; the goal is to determine
graphically whether these two samples come from the same probability
distribution or not. If this is the case, the two samples should be
aggregated in order to increase the robustness of further statistical
analysis.

A QQ-plot is based on the notion of quantile. The
:math:`\alpha`-quantile :math:`q_{X}(\alpha)` of :math:`X`, where
:math:`\alpha \in (0, 1)`, is defined as follows:

.. math::

   \begin{aligned}
       \Prob{ X \leq q_{X}(\alpha)} = \alpha
     \end{aligned}

If a sample :math:`\left\{x_1,\ldots,x_N \right\}` of :math:`X` is
available, the quantile can be estimated empirically:

#. the sample :math:`\left\{x_1,\ldots,x_N \right\}` is first placed in
   ascending order, which gives the sample
   :math:`\left\{ x_{(1)},\ldots,x_{(N)} \right\}`;

#. then, an estimate of the :math:`\alpha`-quantile is:

   .. math::

      \begin{aligned}
            \widehat{q}_{X}(\alpha) = x_{([N\alpha]+1)}
          \end{aligned}

where :math:`[N\alpha]` denotes the integral part of
:math:`N\alpha`.

Thus, the :math:`j^\textrm{th}` smallest value of the sample
:math:`x_{(j)}` is an estimate :math:`\widehat{q}_{X}(\alpha)` of the
:math:`\alpha`-quantile where :math:`\alpha = (j-1)/N`
(:math:`1 < j \leq N`). Let us then consider our second sample
:math:`\left\{x'_1,\ldots,x'_M \right\}`; this one also provides an
estimate :math:`\widehat{q}'_{X}(\alpha)` of this same quantile:

.. math::

   \begin{aligned}
       \widehat{q}'_{X}(\alpha) = x'_{([M\times(j-1)/N]+1)}
     \end{aligned}

If both samples correspond to the same probability distribution,
then :math:`\widehat{q}_{X}(\alpha)` and
:math:`\widehat{q}'_{X}(\alpha)` should be close. Thus, graphically, the
points
:math:`\left\{ \left( \widehat{q}_{X}(\alpha),\widehat{q}'_{X}(\alpha)\right),\  \alpha = (j-1)/N,\ 1 < j \leq N \right\}`
should be close to the diagonal.

The following figure illustrates the principle of a QQ-plot with two
samples of size :math:`M=50` and :math:`N=50`. Note that the unit of the
two axis is that of the variable :math:`X` studied. In this example, the
points remain close to the diagonal and the hypothesis “the two samples
come from the same distribution” does not seem irrelevant, even if a
more quantitative analysis should be carried out to confirm this.

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

    - See :py:func:`~openturns.VisualTest_DrawQQplot` to draw a QQ plot

.. topic:: Examples:

    - See :ref:`examples/data_analysis/qqplot_graph.ipynb`

.. topic:: References:

    - [Saporta1990]_
    - [Dixon1983]_
    - [NISThandbook]_
    - [Dagostino1986]_
    - [Bhattacharyya1997]_
    - [Sprent2001]_
