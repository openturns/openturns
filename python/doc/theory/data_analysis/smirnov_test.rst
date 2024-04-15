.. _smirnov_test:

Two-sample Kolmogorov-Smirnov test
----------------------------------

The Kolmogorov-Smirnov test is a statistical test that can be used
to test whether two given samples of data are drawn from the same
distribution which is of dimension 1 and continuous.

Let :math:`\left\{ x_1,\ldots,x_{\sampleSize} \right\}` and :math:`\left\{ y_1,\ldots,y_{\sampleSize} \right\}`
be two samples of dimension 1 respectively drawn from the (unknown)
distribution functions :math:`F_X` and  :math:`F_Y`.

We want to test  whether both samples are drawn from the same distribution, ie whether :math:`F_X=F_Y`.

This test involves the calculation of the test statistic which is the weighted maximum
distance between both empirical cumulative distribution function
:math:`F_{\sampleSize, X}`  and :math:`F_{\sampleSize, Y}`.
Letting :math:`X_1, \ldots , X_{\sampleSize}` and :math:`Y_1, \ldots , Y_{\sampleSize}`  be independent random variables respectively distributed
according to :math:`X` and :math:`Y`, both empirical cumulative distribution
functions are defined by:

.. math::

    F_{\sampleSize, X}(x) & = \sum_{i=1}^{\sampleSize} 1_{X_i \leq x} \\
    F_{\sampleSize, Y}(x) & = \sum_{i=1}^{\sampleSize} 1_{Y_i \leq x}

for all :math:` x \in \Rset`. The test statistic is defined by:

.. math::

    D_{\sampleSize} = \sqrt{\sampleSize} \sup_x \left|F_{\sampleSize, X}\left(x\right) - F_{\sampleSize, Y}\left(x\right)\right|

The empirical value of the test statistic is denoted by :math:`d`, using the realization of
:math:`F_{\sampleSize, X}`  and :math:`F_{\sampleSize, Y}` on the samples:

.. math::

    F_{\sampleSize, X}(x) & = \dfrac{\mbox{number of } x_i \leq x \mbox{ in the sample}}{\sampleSize}\\
    F_{\sampleSize, Y}(x) & = \dfrac{\mbox{number of } x_i \leq x  \mbox{ in the sample}}{\sampleSize}

Under the null hypothesis :math:`\mathcal{H}_0 = \{ F_X = F_Y\}`, the distribution of the
test statistic :math:`D_{\sampleSize}` is
known: algorithms are available to compute the distribution of :math:`\widehat{D}`
both for :math:`\sampleSize`
large (asymptotic distribution: this is the Kolmogorov distribution) or for
:math:`\sampleSize` small (exact distribution). Then we can use that
distribution to apply the test as follows.
We fix a risk :math:`\alpha`  (error type I) and we evaluate the associated critical value :math:`d_\alpha` which is the quantile of order
:math:`1-\alpha` of :math:`D_{\sampleSize}`.

Then a decision is made, either by comparing the test statistic to the theoretical threshold :math:`d_\alpha`
(or equivalently
by evaluating the p-value of the sample  defined as :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}}` and by comparing
it to :math:`\alpha`):

-  if :math:`d_{\sampleSize}>d_{\alpha}` (or equivalently
   :math:`\Prob{D_{\sampleSize} >  d_{\sampleSize}} < \alpha`), then we reject the
   null hypothesis according to which both samples are drawn from the same distribution,

-  if :math:`d_{\sampleSize} \leq d_{\alpha}` (or equivalently
   :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}} \geq \alpha`),
   then the null hypothesis  is considered acceptable.

.. topic:: API:

    - See :py:func:`~openturns.HypothesisTest.TwoSamplesKolmogorov`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/statistical_tests/plot_smirnov_test`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
