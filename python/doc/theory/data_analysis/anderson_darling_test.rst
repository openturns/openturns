.. _anderson_darling_test:

Anderson-Darling test
---------------------

The Anderson-Darling test is a statistical test of whether a given sample of data is drawn from a given
probability distribution. The library only provides the Anderson-Darling test for normal distributions.

We denote by :math:`\left\{ x_1,\ldots,x_{\sampleSize} \right\}` the data of dimension 1.
Let :math:`F` be  the (unknown) cumulative distribution function of the continuous distribution.

We want to test  whether the sample is drawn from a normal distribution which cumulative distribution function is denoted by :math:`\Phi`.

This test involves the calculation of the test statistic which is
the distance between the empirical cumulative distribution function
:math:`\widehat{F}` built from the sample and :math:`\Phi`. The probability density function of the normal distribution is denoted by :math:`\phi`.

Letting :math:`X_1, \ldots , X_\sampleSize` be i.i.d. random variables following the distribution with CDF :math:`F`, the test statistic is defined by:

.. math::

   \begin{aligned}
       D_{\sampleSize} = \int^{\infty}_{-\infty} \frac{\displaystyle \left[\Phi\left(x\right) - \widehat{F}\left(x\right)\right]^2 }{\displaystyle \Phi(x) \left( 1-\Phi(x) \right) } \, \phi(x)\, dx
     \end{aligned}

This distance is a quadratic
type, as in the :ref:`Cramer-Von Mises test <cramer_vonmises_test>`,
but gives more weight to deviations of extreme values. The empirical value of the test statistic, evaluated from the sample is:

.. math::

   \begin{aligned}
       d_{\sampleSize} = -\sampleSize-\sum^{\sampleSize}_{i=1} \frac{2i-1}{\sampleSize} \left[\log \Phi(x_{(i)})+\log\left(1-\Phi(x_{(N+1-i)})\right)\right]
     \end{aligned}

where :math:`\left\{x_{(1)}, \ldots, x_{(\sampleSize)}\right\}` is the sample sorted in ascending order:

.. math::

   x_{(1)} \leq \dots \leq x_{(\sampleSize)}

Under the null hypothesis :math:`\mathcal{H}_0 = \{ G = \Phi\}`, the distribution of the test statistic :math:`D_{\sampleSize}` is
asymptotically known i.e. when :math:`\sampleSize \rightarrow +\infty`.
If :math:`\sampleSize` is sufficiently large, we can use the asymptotic distribution to apply the test
as follows.
We fix a risk :math:`\alpha` (error type I) and we evaluate the associated critical value :math:`d_\alpha` which is the quantile of order
:math:`(1-\alpha)` of :math:`D_{\sampleSize}`.

Then a decision is made, either by comparing the test statistic to the theoretical threshold :math:`d_\alpha`
(or equivalently
by evaluating the p-value of the sample  defined as :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}}` and by comparing
it to :math:`\alpha`):

-  if :math:`d_{\sampleSize}>d_{\alpha}` (or equivalently :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}} \leq \alpha`),
   then we reject the normal distribution,

-  if :math:`d_{\sampleSize} \leq d_{\alpha}` (or equivalently :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}} \geq \alpha`),
   then the normal distribution is considered acceptable.




.. topic:: API:

    - See :py:func:`~openturns.NormalityTest.AndersonDarlingNormal`

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
