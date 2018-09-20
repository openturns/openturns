.. _graphical_fitting_test:

Graphical goodness-of-fit tests
-------------------------------

This method deals with the modelling of a probability distribution of a
random vector :math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`. It
seeks to verify the compatibility between a sample of data
:math:`\left\{ \vect{x}_1,\vect{x}_2,\ldots,\vect{x}_N \right\}` and a
candidate probability distribution previous chosen.
The use of graphical tools allows to answer this question in the one
dimensional case :math:`n_X =1`, and with a continuous distribution.
The QQ-plot, and henry line tests are defined in the case to
:math:`n_X = 1`. Thus we denote :math:`\vect{X} = X^1 = X`. The first
graphical tool provided is a QQ-plot (where “QQ” stands
for “quantile-quantile”). In the specific case of a Normal distribution,
Henry’s line may also be used.

**QQ-plot**

A QQ-Plot is based on the notion of quantile. The
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
(:math:`1 < j \leq N`).

Let us then consider the candidate probability distribution being
tested, and let us denote by :math:`F` its cumulative distribution
function. An estimate of the :math:`\alpha`-quantile can be also
computed from :math:`F`:

.. math::

   \begin{aligned}
       \widehat{q}'_{X}(\alpha) = F^{-1} \left( (j-1)/N \right)
     \end{aligned}

If :math:`F` is really the cumulative distribution function of
:math:`F`, then :math:`\widehat{q}_{X}(\alpha)` and
:math:`\widehat{q}'_{X}(\alpha)` should be close. Thus, graphically, the
points
:math:`\left\{ \left( \widehat{q}_{X}(\alpha),\widehat{q}'_{X}(\alpha)\right),\  \alpha = (j-1)/N,\ 1 < j \leq N \right\}`
should be close to the diagonal.

The following figure illustrates the principle of a QQ-plot with a
sample of size :math:`N=50`. Note that the unit of the two axis is that
of the variable :math:`X` studied; the quantiles determined via
:math:`F` are called here “value of :math:`T`”. In this example, the
points remain close to the diagonal and the hypothesis “:math:`F` is the
cumulative distribution function of :math:`X`” does not seem irrelevant,
even if a more quantitative analysis (see for instance ) should be
carried out to confirm this.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    distribution = ot.Normal(3.0, 2.0)
    sample = distribution.getSample(150)
    graph = ot.VisualTest.DrawQQplot(sample, distribution)
    View(graph)

In this second example, the candidate distribution function is clearly
irrelevant.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    distribution = ot.Normal(3.0, 3.0)
    distribution2 = ot.Normal(2.0, 1.0)
    sample = distribution.getSample(150)
    graph = ot.VisualTest.DrawQQplot(sample, distribution2)
    View(graph)


**Henry’s line**

This second graphical tool is only relevant if the candidate
distribution function being tested is gaussian. It also uses the ordered
sample :math:`\left\{ x_{(1)},\ldots,x_{(N)} \right\}` introduced for
the QQ-plot, and the empirical cumulative distribution function
:math:`\widehat{F}_N` presented in .

By definition,

.. math::

   \begin{aligned}
       x_{(j)} = \widehat{F}_N^{-1} \left( \frac{j}{N} \right)
     \end{aligned}

Then, let us denote by :math:`\Phi` the cumulative distribution
function of a Normal distribution with mean 0 and standard deviation 1.
The quantity :math:`t_{(j)}` is defined as follows:

.. math::

   \begin{aligned}
       t_{(j)} = \Phi^{-1} \left( \frac{j}{N} \right)
     \end{aligned}

If :math:`X` is distributed according to a normal probability
distribution with mean :math:`\mu` and standard-deviation
:math:`\sigma`, then the points
:math:`\left\{ \left( x_{(j)},t_{(j)} \right),\ 1 \leq j \leq N \right\}`
should be close to the line defined by :math:`t = (x-\mu) / \sigma`.
This comes from a property of a normal distribution: it the distribution
of :math:`X` is really :math:`\cN(\mu,\sigma)`, then the distribution of
:math:`(X-\mu) / \sigma` is :math:`\cN(0,1)`.

The following figure illustrates the principle of Henry’s graphical test
with a sample of size :math:`N=50`. Note that only the unit of the
horizontal axis is that of the variable :math:`X` studied. In this
example, the points remain close to a line and the hypothesis “the
distribution function of :math:`X` is a Gaussian one” does not seem
irrelevant, even if a more quantitative analysis (see for instance )
should be carried out to confirm this.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    distribution = ot.Normal(10.0, 2.0)
    sample = distribution.getSample(50)
    graph = ot.VisualTest.DrawHenryLine(sample)
    View(graph)

In this example the test validates the hypothesis of a gaussian distribution.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    distribution = ot.LogNormal(2.0, 1.0, 0.0)
    sample = distribution.getSample(50)
    graph = ot.VisualTest.DrawHenryLine(sample)
    View(graph)

In this second example, the hypothesis of a gaussian distribution seems
far less relevant because of the behaviour for small values of
:math:`X`.

**Kendall plot**

In the bivariate case, the Kendall Plot test enables to validate the
choice of a specific copula model or to verify that two samples share
the same copula model.

Let :math:`\vect{X}` be a bivariate random vector which copula is
noted :math:`C`.
Let :math:`(\vect{X}^i)_{1 \leq i \leq N}` be a sample of
:math:`\vect{X}`.

We note:

.. math::

   \begin{aligned}
     \forall i \geq 1, \displaystyle H_i = \frac{1}{n-1} Card \left\{  j \in [1,N], j  \neq i, \, | \, x^j_1 \leq x^i_1 \mbox{ and } x^j_2 \leq x^i_2  \right \}
   \end{aligned}

and :math:`(H_{(1)}, \dots, H_{(N)})` the ordered statistics of
:math:`(H_1, \dots, H_N)`.

The statistic :math:`W_i` is defined by:

.. math::
    :label: Wi

    W_i = N C_{N-1}^{i-1} \int_0^1 t K_0(t)^{i-1} (1-K_0(t))^{n-i} \, dK_0(t)

where :math:`K_0(t)` is the cumulative density function of
:math:`H_i`. We can show that this is the cumulative density function
of the random variate :math:`C(U,V)` when :math:`U` and :math:`V` are
independent and follow :math:`Uniform(0,1)` distributions.

| Equation :eq:`Wi` is evaluated with the Monte Carlo
  sampling method : it generates :math:`n` samples of size
  :math:`N` from the bivariate copula :math:`C`, in order to have
  :math:`n` realisations of the statistics
  :math:`H_{(i)},\forall 1 \leq i \leq N` and have an estimation of
  :math:`W_i = E[H_{(i)}], \forall i \leq N`.

| When testing a specific copula with respect to a sample, the Kendall
  Plot test draws the points :math:`(W_i, H_{(i)})_{1 \leq i \leq N}`.
  If the points are one the first diagonal, the copula model is
  validated.
| When testing whether two samples have the same copula, the Kendall
  Plot test draws the points
  :math:`(H^1_{(i)}, H^2_{(i)})_{1 \leq i \leq N}` respectively
  associated to the first and second sample. Note that the two samples
  must have the same size.


.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    copula = ot.FrankCopula(1.5)
    sample = copula.getSample(100)
    graph = ot.VisualTest.DrawKendallPlot(sample, copula)
    View(graph)

The Kendall Plot test validates the use of the Frank copula for a sample.


.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    copula = ot.FrankCopula(1.5)
    copula2 = ot.GumbelCopula(4.5)
    sample = copula.getSample(100)
    graph = ot.VisualTest.DrawKendallPlot(sample, copula2)
    View(graph)

The Kendall Plot test invalidates the use of the Frank copula for a sample. 


Remark: In the case where you want to test a sample with respect to a
specific copula, if the size of the sample is superior to 500, we
recommend to use the second form of the Kendall plot test: generate a
sample of the proper size from your copula and then test both samples.
This way of doing is more efficient.

.. topic:: API:

    - See :py:func:`~openturns.VisualTest_DrawQQplot` to draw a QQ plot
    - See :py:func:`~openturns.VisualTest_DrawHenryLine` to draw the Henry line
    - See :py:func:`~openturns.VisualTest_DrawKendallPlot` to draw the Kendall plot

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/qqplot_graph`
    - See :doc:`/examples/data_analysis/henry_line_graph`
    - See :doc:`/examples/data_analysis/kendallplot_test`

.. topic:: References:

    - [Saporta1990]_
    - [Dixon1983]_
