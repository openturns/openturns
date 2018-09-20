.. _monte_carlo_moments:

Estimating moments with Monte Carlo
-----------------------------------

Let us denote
:math:`\underline{Y} = h\left( \vect{X},\vect{d} \right) = \left( Y^1,\ldots,Y^{n_Y} \right)`,
where :math:`\vect{X}= \left( X^1,\ldots,X^{n_X} \right)` is a random
vector, and :math:`\vect{d}` a deterministic vector. We seek here to
evaluate, the characteristics of the central part (central tendency and
spread i.e. mean and variance) of the probability distribution of a
variable :math:`Y^i`, using the probability distribution of the random
vector :math:`\vect{X}`.

The Monte Carlo method is a numerical integration method using sampling,
which can be used, for example, to determine the mean and standard
deviation of a random variable :math:`Y^i` (if these quantities exist,
which is not the case for all probability distributions):

.. math::

   \begin{aligned}
       m_{Y^i} = \int u \, f_{Y^i}(u) \, du,\ \sigma_{Y^i} = \sqrt{\int \left( u-m_{Y^i} \right)^2 \, f_{Y^i}(u) \, du}
     \end{aligned}

where :math:`f_{Y^i}` represents the probability density function of
:math:`Y^i`.

Suppose now that we have the sample
:math:`\left\{ y^i_1,\ldots,y^i_N \right\}` of :math:`N` values randomly
and independently sampled from the probability distribution
:math:`f_{Y^i}`; this sample can be obtained by drawing a :math:`N`
sample :math:`\left\{ \vect{x}_1,\ldots,\vect{x}_N \right\}` of the
random vector :math:`\vect{X}` (the distribution of which is known) and
by computing
:math:`\vect{y}_j = h \left( \vect{x}_j,\underline{d} \right) \ \forall 1 \leq j \leq N`.
Then, the Monte-Carlo estimations for the mean and standard deviation
are the empirical mean and standard deviations of the sample:

.. math::

   \begin{aligned}
       \widehat{m}_{Y^i} = \frac{1}{N} \sum_{j=1}^N y^i_j,\ \widehat{\sigma}_{Y^i} = \sqrt{\frac{1}{N} \sum_{j=1}^N \left( y^i_j - \widehat{m}_{Y^i} \right)^2}
     \end{aligned}

These are just estimations, but by the law of large numbers their
convergence to the real values :math:`m_{Y^i}` and :math:`\sigma_{Y^i}`
is assured as the sample size :math:`N` tends to infinity. The Central
Limit Theorem enables the difference between the estimated value and the
sought value to be controlled by means of a confidence interval
(especially if N is sufficiently large, typically :math:`N` > a few
dozens even if there is now way to say for sure if the asymptotic
behaviour is reached). For a probability :math:`\alpha` strictly between
0 and 1 chosen by the user, one can, for example, be sure with a
confidence :math:`\alpha`, that the true value of :math:`m_{Y^i}` is
between :math:`\widehat{m}_{i,\inf}` and :math:`\widehat{m}_{i,\sup}`
calculated analytically from simple formulae. To illustrate, for
:math:`\alpha = 0.95`:

.. math::

   \begin{aligned}
       \widehat{m}_{i,\inf} = \widehat{m}_{Y^i} - 1.96 \frac{\displaystyle \widehat{\sigma}_{Y^i}}{\displaystyle \sqrt{N}},\ \widehat{m}_{i,\sup} = \widehat{m}_{Y^i} + 1.96 \frac{\widehat{\sigma}_{Y^i}}{\sqrt{N}},\ \textrm{that is to say}\ \textrm{Pr} \left(  \widehat{m}_{i,\inf} \leq m_{Y^i} \leq \widehat{m}_{i,\sup} \right) = 0.95
     \end{aligned}

The size of the confidence interval, which represents the uncertainty
of this mean estimation, decreases as :math:`N` increases but more
gradually (the rate is proportional to :math:`\sqrt{N}`: multiplying
:math:`N` by 100 reduces the length of the confidence interval
:math:`\left| \widehat{m}_{i,\inf}-\widehat{m}_{i,\sup} \right|` by a
factor 10).

This method is also referred to as Direct sampling, crude Monte Carlo method, Classical Monte Carlo
integration.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    dist = ot.LogNormal()
    dist.setDescription(['Z'])
    sample = dist.getSample(100)
    graph = dist.drawPDF()
    histogram = ot.VisualTest.DrawHistogram(sample).getDrawable(0)
    histogram.setColor('blue')
    graph.add(histogram)
    View(graph)

.. topic:: API:

    - See :class:`~openturns.Sample`

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/estimate_moments`


.. topic:: References:

    - Robert C.P., Casella G. (2004). "Monte Carlo Statistical Methods", Springer, ISBN 0-387-21239-6, 2nd ed.
    - Rubinstein R.Y. (1981). "Simulation and The Monte Carlo methods", John Wiley \& Sons
    - "Guide to the expression of Uncertainty in Measurements (GUM)", ISO publication

