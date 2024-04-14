.. _chi2_fitting_test:

Chi-squared test
----------------

The :math:`\chi^2` test is a statistical test of whether a given sample of data is drawn
from a given discrete distribution. The library only provides the :math:`\chi^2` test for
distributions of dimension 1.

We denote by :math:`\left\{ x_1,\dots,x_{\sampleSize} \right\}` a sample of dimension 1.
Let :math:`F` be  the (unknown) cumulative distribution function of the discrete distribution.

We want to
test whether the sample is drawn from the discrete distribution characterized by the
probabilities :math:`\left\{ p(x;\vect{\theta}) \right\}_{x \in \cE}` and
which cumulative distribution function is denoted by :math:`G`.

This test  involves the calculation of the test statistic which is
the distance between the empirical number of values equal to :math:`x` in the sample and the
theoretical mean one evaluated from the discrete distribution.

Let :math:`X_1, \ldots , X_{\sampleSize}` be i.i.d. random variables following the
distribution with CDF :math:`F`. According to the tested distribution :math:`G`,
the theoretical mean number of values equal to :math:`x` is :math:`\sampleSize p(x;\vect{\theta})`
whereas the number evaluated from the :math:`X_1, \ldots , X_{\sampleSize}` is
:math:`N(x) = \sum_{i=1}^{\sampleSize} 1_{X_i=x}`.
Then the test statistic is defined by:

  .. math::

         D_{\sampleSize} = \sum_{x \in \cE} \frac{\left[\sampleSize p(x)-N(x)\right]^2}{N(x)}

If some values of :math:`x` have not been observed in the sample, we have to gather values in
classes so that they contain at least 5 data points (empirical rule). Then the theoretical
probabilities of all the values in the class are added to get the
theoretical probability of the class.

Let :math:`d_{\sampleSize}` be the realization of the test statistic :math:`d_{\sampleSize}`
on the sample :math:`\left\{ x_1,\dots,x_{\sampleSize} \right\}`.

Under the null hypothesis :math:`\mathcal{H}_0 = \{ G = F\}`,
the distribution of the test statistic :math:`D_{\sampleSize}` is
known: this is the :math:`\chi^2(J-1)` distribution, where :math:`J` is the number
of distinct values in the support of :math:`G`.
We apply the test as follows.

We fix a risk :math:`\alpha` (error type I) and we evaluate the associated critical value
:math:`d_\alpha` which is the quantile of order :math:`(1-\alpha)` of :math:`D_{\sampleSize}`.

Then a decision is made, either by comparing the test statistic to the theoretical threshold
:math:`d_\alpha` (or equivalently by evaluating the p-value of the sample  defined as
:math:`\Prob{D_{\sampleSize} > d_{\sampleSize}}` and by comparing it to :math:`\alpha`):

-  if :math:`d_{\sampleSize}>d_{\alpha}` (or equivalently
   :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}} \leq \alpha`),
   then we reject :math:`G`,

-  if :math:`d_{\sampleSize} \leq d_{\alpha}` (or equivalently
   :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}} \geq \alpha`),
   then :math:`G` is considered acceptable.


.. topic:: API:

    - See :py:func:`~openturns.FittingTest.ChiSquared`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/statistical_tests/plot_chi2_fitting_test`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
    - [nisthandbook]_
    - [dagostino1986]_
    - [sprent2001]_
    - [bhattacharyya1997]_
    - [burnham2002]_
