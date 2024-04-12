.. _smirnov_test:

Two-sample Kolmogorov-Smirnov test
----------------------------------

The Kolmogorov-Smirnov test is a statistical test that can be used
to test whether two given samples of data are drawn from the same
distribution which is of dimension 1 and continuous.

We denote by :math:`\left\{ x_1,\ldots,x_{\sampleSize} \right\}`
and :math:`\left\{ y_1,\ldots,y_{\sampleSize} \right\}` two given samples of dimension 1.
Let :math:`F` be  the (unknown) cumulative distribution function of
:math:`\left\{ x_1,\ldots,x_{\sampleSize} \right\}` and :math:`G` the one
of :math:`\left\{ y_1,\ldots,y_{\sampleSize} \right\}`.

We want to test  whether both samples are drawn from the same distribution, ie whether :math:`F=G`.


This test involves the calculation of the test statistic which is the weighted maximum
distance between both empirical cumulative distribution functions
:math:`\widehat{F}_{X}` and :math:`\widehat{F}_{Y}` built respectively from
:math:`\left\{ x_1,\ldots,x_{\sampleSize} \right\}`
and :math:`\left\{ y_1,\ldots,y_{\sampleSize} \right\}`.
Letting :math:`X_1, \ldots , X_\sampleSize` be i.i.d. random variables following the distribution
with CDF :math:`F`, and :math:`Y_1, \ldots , Y_\sampleSize` be i.i.d. random variables following
the distribution with CDF :math:`G`, the test statistic is defined by:

.. math::

    D_{\sampleSize} = \sqrt{\sampleSize} \sup_x \left|\widehat{F}_{X}\left(x\right) - \widehat{F}_{Y}\left(x\right)\right|

The empirical value of the test statistic, evaluated from the sample is:

.. math::

    d = \sqrt{\sampleSize} \sup_{x_i}
    \left|\widehat{F}_{X}\left(x_i\right) - \widehat{F}_{Y}\left(y_i\right)\right|

Under the null hypothesis :math:`\mathcal{H}_0 = \{ G = F\}`, the distribution of the
test statistic :math:`D_{\sampleSize}` is
known: algorithms are available to compute the distribution of :math:`\widehat{D}`
both for :math:`\sampleSize`
large (asymptotic distribution: this is the Kolmogorov distribution) or for
:math:`\sampleSize` small (exact distribution). Then we can use that
distribution to apply the test as follows.
We fix a risk :math:`\alpha`  (error type I) and we evaluate the associated critical value :math:`d_\alpha` which is the quantile of order
:math:`(1-\alpha)` of :math:`D_{\sampleSize}`.

Then a decision is made, either by comparing the test statistic to the theoretical threshold :math:`d_\alpha`
(or equivalently
by evaluating the p-value of the sample  defined as :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}}` and by comparing
it to :math:`\alpha`):

-  if :math:`d_{\sampleSize}>d_{\alpha}` (or equivalently
   :math:`\Prob{D_{\sampleSize} >  d_{\sampleSize}} \leq \alpha`), then we reject the
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
