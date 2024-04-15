.. _cramer_vonmises_test:

Cramer-Von Mises test
---------------------

The Cramer-Von Mises test is a statistical test of whether a given sample of data is drawn from a given
probability distribution which is of dimension 1 and continuous.

We denote by :math:`\left\{ x_1,\ldots, x_{\sampleSize} \right\}` the data of dimension 1.
Let :math:`F` be  the (unknown) cumulative distribution function of the continuous distribution.

We want to test  whether the sample is drawn from the cumulative distribution function :math:`G`.

This test involves the calculation of the test statistic which is
the integrated squared distance between the empirical cumulative distribution function
:math:`\widehat{F}` built from the sample and :math:`G`.
Letting :math:`X_1, \ldots , X_\sampleSize` be i.i.d. random variables following the distribution with CDF :math:`F`, the test statistic is defined by:

.. math::

   \begin{aligned}
       D_{\sampleSize} = \int^{\infty}_{-\infty} \left[G\left(x\right) - \widehat{F}\left(x\right)\right]^2 \,
       p\left(x\right) dx
     \end{aligned}

The empirical value of the test statistic, evaluated from the sample is:

.. math::

   \begin{aligned}
       d_{\sampleSize} = \frac{1}{12 \sampleSize} + \sum_{i=1}^{\sampleSize}\left[\frac{2i-1}{2\sampleSize} -
       G\left(x_i\right)\right]^2
     \end{aligned}


Under the null hypothesis :math:`\mathcal{H}_0 = \{ G = F\}`, the distribution of the test statistic :math:`D_{\sampleSize}` is
asymptotically known i.e. when :math:`\sampleSize \rightarrow +\infty`.
If :math:`\sampleSize` is sufficiently large, we can use the asymptotic distribution to apply the
test as follows.
We fix a risk :math:`\alpha` (error type I) and we evaluate the associated critical value :math:`d_\alpha` which is the quantile of order
:math:`1-\alpha` of :math:`D_{\sampleSize}`.

Then a decision is made, either by comparing the test statistic to the theoretical threshold :math:`d_\alpha`
(or equivalently
by evaluating the p-value of the sample  defined as :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}}` and by comparing
it to :math:`\alpha`):

-  if :math:`d_{\sampleSize}>d_{\alpha}` (or equivalently :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}} < \alpha`),
   then we reject :math:`G`,

-  if :math:`d_{\sampleSize} \leq d_{\alpha}` (or equivalently :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}} \geq \alpha`),
   then :math:`G` is considered acceptable.


.. topic:: API:

    - See :py:func:`~openturns.NormalityTest.CramerVonMisesNormal`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/statistical_tests/plot_test_normality`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
    - [nisthandbook]_
    - [dagostino1986]_
    - [sprent2001]_
    - [bhattacharyya1997]_
    - [burnham2002]_
