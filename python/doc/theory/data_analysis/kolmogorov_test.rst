.. _kolmogorov_smirnov_test:

The Kolmogorov-Smirnov goodness of fit test for continuous data
---------------------------------------------------------------

The Kolmogorov-Smirnov test is a statistical test of whether a given sample of data is drawn from a given
probability distribution which is of dimension 1 and continuous.

Let :math:`\left\{ x_1,\ldots, x_{\sampleSize} \right\}` be a sample of dimension 1 drawn from the (unknown) cumulative distribution function :math:`F`.

We want to test  whether the sample is drawn from the cumulative distribution function
:math:`G`.

This test involves the calculation of the test statistic which is the weighted maximum
distance between the empirical cumulative distribution function
:math:`F_{\sampleSize}` and :math:`G`.
Letting :math:`X_1, \ldots , X_{\sampleSize}`  be independent random variables respectively distributed according to :math:`F`, then :math:`F_{\sampleSize}` is defined by:

.. math::

    F_{\sampleSize}(x) & = \sum_{i=1}^{\sampleSize} 1_{X_i \leq x}

for all :math:`x \in \Rset`. The test statistic is defined by:

.. math::

    D_{\sampleSize} = \sqrt{\sampleSize} \sup_{x} \left|F_{\sampleSize}\left(x \right) - G\left(x \right)\right|

The empirical value of the test statistic is denoted by :math:`d`, using the realization of
:math:`F_{\sampleSize}` on the sample:

.. math::

    F_{\sampleSize}(x) & = \dfrac{\mbox{number of } x_i \leq x \mbox{ in the sample}}{\sampleSize}

Under the null hypothesis :math:`\mathcal{H}_0 = \{ G = F\}`, the distribution of
the test statistic :math:`D_{\sampleSize}` is
known: algorithms are available to compute the distribution of :math:`D_{\sampleSize}`
both for :math:`\sampleSize`
large (asymptotic distribution: this is the Kolmogorov distribution) or for
:math:`\sampleSize` small (exact distribution). Then we can use that
distribution to apply the test as follows.
We fix a risk :math:`\alpha` (error type I) and we evaluate the associated critical
value :math:`d_\alpha` which is the quantile of order
:math:`1-\alpha` of :math:`D_{\sampleSize}`.

Then a decision is made, either by comparing the test statistic to the theoretical
threshold :math:`d_\alpha`
(or equivalently
by evaluating the p-value of the sample  defined as
:math:`\Prob{D_{\sampleSize} > d_{\sampleSize}}` and by comparing
it to :math:`\alpha`):

-  if :math:`d_{\sampleSize}>d_{\alpha}` (or equivalently
   :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}} < \alpha`),
   then we reject :math:`G`,

-  if :math:`d_{\sampleSize} \leq d_{\alpha}` (or equivalently
   :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}} \geq \alpha`),
   then :math:`G` is considered acceptable.

It is assumed that the parameters of the continuous distribution which is tested have
not been inferred from the sample. If this is the case, we have to use the Lilliefors
test rather than the Kolmogorov test.

The figure below illustrates the Kolmogorov-Smirnov test for an ordered sample
:math:`\left\{5,6,10,22,27\right\}` with respect to the Exponential distribution
parameterized by :math:`\lambda = 0.07`, :math:`\gamma = 0`.

.. plot::

    import openturns as ot
    from matplotlib import pyplot as plt
    import openturns.viewer as otv

    candidate = ot.Exponential(0.07, 0.0)
    graph = candidate.drawCDF(0.0, 30.0)

    sample = ot.Sample([[5.0], [6.0], [10.0], [22.0], [27.0]])
    empiricalDrawable = ot.UserDefined(sample).drawCDF(0.0, 30.0).getDrawable(0)
    empiricalDrawable.setColor('darkblue')
    graph.add(empiricalDrawable)

    graph.setTitle('CDF comparison')
    graph.setLegends(['Candidate CDF', 'Empirical CDF'])
    otv.View(graph)


.. topic:: API:

    - See :py:func:`~openturns.FittingTest.Kolmogorov` to compare a sample and a given distribution,
    - See :py:func:`~openturns.FittingTest.BestModelKolmogorov` to select the best model,
    - See :py:func:`~openturns.HypothesisTest.TwoSamplesKolmogorov` to compare two samples
    - See :py:func:`~openturns.FittingTest.Lilliefors` to compare a sample  and a given distribution
      whose parameters have been estimated from the sample

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/statistical_tests/plot_kolmogorov_test`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
    - [nisthandbook]_
    - [dagostino1986]_
    - [sprent2001]_
    - [bhattacharyya1997]_
    - [Lilliefors1967]_
    - [Keutelian1991]_
    - [AbdiMolinSalkind2007]_
    - [AbdiMolin1998]_
    - [ScottStewart2011]_
    - [NikitinTchirina2007]_

