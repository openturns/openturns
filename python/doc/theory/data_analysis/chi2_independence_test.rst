.. _chi2_independence_test:

Chi-squared test for independence
---------------------------------

The :math:`\chi^2` test can be used to detect dependencies between two random discrete variables.

Let :math:`\vect{X} = (X^1, X^2)` be a random variable of dimension 2 with  values in
:math:`\{b_1, \dots, b_{\ell} \} \times \{c_1, \dots, c_{r} \}`.

We want to test whether :math:`\vect{X}` has independent components.

Let :math:`\vect{X}_1, \ldots , \vect{X}_\sampleSize` be i.i.d. random variables following the distribution of :math:`\vect{X}`. Two test statistics can be defined by:

.. math::

       D_{\sampleSize}^{(1)}  = \sum_{i=1}^{\ell} \sum_{j=1}^{r} \dfrac{\left(N_{i,j} -
       \frac{N_{i,.}N_{.,j}}{\sampleSize}\right)}{N_{i,j}} \\
       D_{\sampleSize}^{(2)}  = \sampleSize \sum_{i=1}^{\ell} \sum_{j=1}^{r}
       \dfrac{\left(N_{i,j} - \frac{N_{i,.}N_{.,j}}{\sampleSize}\right)}{N_{i,.}N_{.,j}}


where:

-  :math:`N_{i,j} = \sum_{k=1}^{\sampleSize}1_{X^1_k = b_i, X^2_k = c_j}` be the number of pairs
   equal to :math:`(b_i, c_j)`,

-  :math:`N_{i,.}= \sum_{k=1}^{\sampleSize}1_{X^1_k = b_i}` be the number of pairs
   such that the first component is equal to :math:`b_i`,

-  :math:`N_{., j}= \sum_{k=1}^{\sampleSize}1_{X^2_k = c_j}` be the number of pairs
   such that the second component is equal to :math:`c_j`.

Let :math:`d_{\sampleSize}^{(i)}` be the realization of the test statistic
:math:`D_{\sampleSize}^{(i)}` on the sample
:math:`\left\{ \vect{x}_1,\dots,\vect{x}_{\sampleSize} \right\}` with :math:`i=1,2`.

Under the null hypothesis :math:`\mathcal{H}_0 = \{ \vect{X} \mbox{ has independent components}\}`,
the distribution of both test statistics :math:`D_{\sampleSize}^{(i)}` is asymptotically
known: i.e. when :math:`\sampleSize \rightarrow +\infty`: this is
the :math:`\chi^2((\ell-1)(r-1))` distribution.
If :math:`\sampleSize` is sufficiently large, we can use the asymptotic distribution to apply
the test as follows.

We fix a risk :math:`\alpha` (error type I) and we evaluate the associated critical value
:math:`d_\alpha` which is the quantile of order
:math:`1-\alpha` of :math:`D_{\sampleSize}^{(i)}`.

Then a decision is made, either by comparing the test statistic to the theoretical threshold
:math:`d_\alpha^{(i)}` (or equivalently by evaluating the p-value of the sample  defined as
:math:`\Prob{D_{\sampleSize}^{(i)} > d_{\sampleSize}^{(i)}}` and by comparing it to :math:`\alpha`):

-  if :math:`d_{\sampleSize}^{(i)}>d_{\alpha}^{(i)}` (or equivalently
   :math:`\Prob{D_{\sampleSize}^{(i)} > d_{\sampleSize}^{(i)}} < \alpha`),
   then we reject the independence between the components,

-  if :math:`d_{\sampleSize}^{(i)} \leq d_{\alpha}^{(i)}` (or equivalently
   :math:`\Prob{D_{\sampleSize}^{(i)} > d_{\sampleSize}^{(i)}} \geq \alpha`),
   then the independence between the components is considered acceptable.


.. topic:: API:

    - See :py:func:`~openturns.HypothesisTest.ChiSquared`


.. topic:: Examples:

    - See :doc:`/auto_data_analysis/statistical_tests/plot_test_independence`


.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
    - [nisthandbook]_
    - [dagostino1986]_
    - [bhattacharyya1997]_
    - [sprent2001]_
    - [burnham2002]_
