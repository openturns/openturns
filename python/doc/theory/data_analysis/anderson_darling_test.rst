.. _anderson_darling_test:

Anderson-Darling test
---------------------

The Anderson-Darling test is a statistical test of whether a given sample of data is drawn from a given
probability distribution. The library only provides the Anderson-Darling test for normal distributions.

Let :math:`\left\{ x_1,\ldots,x_{\sampleSize} \right\}` be a sample of dimension 1 drawn from the (unknown) cumulative distribution function :math:`F` assumed to be continuous.
We want to test  whether the sample is drawn from a normal distribution ie whether
:math:`F_X = \Phi`, where :math:`\Phi` is the cumulative distribution function of the normal
distribution.

This test involves the calculation of the test statistic which is
the distance between the empirical cumulative distribution function
:math:`F_{\sampleSize}` and :math:`\Phi`. Letting :math:`X_1, \ldots , X_{\sampleSize}`
be independent random variables respectively distributed according to :math:`F`, we define
the the order statistics :math:`X_{(1)}, \ldots , X_{(\sampleSize)}` by:

.. math::

   X_{(1)} \leq \dots \leq X_{(\sampleSize)}.

The test statistic is defined by:

.. math::

       D_{\sampleSize} = -\sampleSize-\sum^{\sampleSize}_{i=1} \frac{2i-1}{\sampleSize} \left[\log \left( \Phi(X_{(i)}) \right) + \log\left(1-\Phi(X_{(\sampleSize+1-i)})\right)\right].

This distance is a quadratic
type, as in the :ref:`Cramer-Von Mises test <cramer_vonmises_test>`,
but gives more weight to deviations of tail values. The empirical value of the
test statistic denoted by :math:`d_{\sampleSize}` is evaluated from the sample sorted
in ascending order:

.. math::

   x_{(1)} \leq \dots \leq x_{(\sampleSize)}.

Under the null hypothesis :math:`\mathcal{H}_0 = \{ F = \Phi\}`, the asymptotic
distribution of the test statistic :math:`D_{\sampleSize}` is known i.e. when
:math:`\sampleSize \rightarrow +\infty`.
If :math:`\sampleSize` is sufficiently large, we can use the asymptotic distribution
to apply the test
as follows.
We fix a risk :math:`\alpha` (error type I) and we evaluate the associated critical
value :math:`d_\alpha` which is the quantile of order
:math:`1-\alpha` of :math:`D_{\sampleSize}`.

Then a decision is made, either by comparing the test statistic to the theoretical threshold :math:`d_\alpha`
(or equivalently
by evaluating the p-value of the sample  defined as :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}}` and by comparing
it to :math:`\alpha`):

-  if :math:`d_{\sampleSize}>d_{\alpha}` (or equivalently :math:`\Prob{D_{\sampleSize} > d_{\sampleSize}} < \alpha`),
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
