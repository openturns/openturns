.. _kolmogorov_smirnov_test:

Kolmogorov-Smirnov fitting test
-------------------------------

This method deals with the modelling of a probability distribution of a random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`.

It seeks to verify the compatibility between a sample of data
:math:`\left\{ \vect{x}_1,\vect{x}_2,\ldots,\vect{x}_N \right\}`
and a candidate probability distribution previously chosen.

The Kolmogorov-Smirnov Goodness-of-Fit test allows to answer this question in
the one dimensional case :math:`n_X =1`, and with a continuous distribution.

Let us limit the case to :math:`n_X = 1`.

Thus we denote :math:`\vect{X} = X^1 = X`.
This goodness-of-fit test is based on the maximum distance between the
cumulative distribution function :math:`\widehat{F}_N` of the sample
:math:`\left\{ x_1,x_2,\ldots,x_N \right\}` and that of the candidate
distribution, denoted *F*.

This distance may be expressed as follows:

.. math::

    D = \sup_x \left|\widehat{F}_N\left(x\right) - F\left(x\right)\right|

With a sample :math:`\left\{ x_1,x_2,\ldots,x_N \right\}`, the distance is estimated by:

.. math::

    \widehat{D}_N = \sup_{i=1 \ldots N}\left|F\left(x_i\right)-\frac{i-1}{N} ; \frac{i}{N}-F\left(x_i\right)\right|

Assume that the sample is drawn from the candidate distribution. 
By definition, the *p*-value of the test is the probability:

.. math::

    p = P(D > \widehat{D}_N)

In the case where the fit is good, the value of :math:`\widehat{D}_N` is 
small, which leads to a p-value closer to 1. 
The candidate distribution will not be rejected if and only if 
:math:`p` is larger than a given threshold probability. 
In general, the threshold p-value is chosen to be 0.05:

.. math::

    p_{ref} = 0.05

Based on the p-value, 

- if  :math:`p<p_{ref}`, we reject the candidate distribution,

- otherwise, the candidate distribution is not rejected.

Two situations may occur in practice.

- the parameters of the distribution under test are known,

- the parameters of the distribution under test are estimated from a sample. 

If the parameters of the distribution under test are known, 
algorithms are available to directly compute 
the distribution of :math:`\widehat{D}_N` both for *N* large 
(asymptotic distribution) or for *N* small (exact distribution). 
This is because the distribution of :math:`\widehat{D}_N` does 
not depend on the candidate distribution. 

If the parameters of the distribution under test are estimated 
from a sample, the :math:`\widehat{D}_N` statistic is generally smaller, because 
the parameters of the distribution have been computed from the 
sample. 
In general, the distribution of :math:`\widehat{D}_N` is not known 
and depends on the candidate distribution.  
Therefore, sampling methods can be used in order to estimate the p-value.  

The diagram below illustrates the principle of comparison with the empirical
cumulative distribution function for an ordered sample
:math:`\left\{5,6,10,22,27\right\}`; the candidate distribution considered here
is the Exponential distribution with parameters :math:`\lambda = 0.07`,
:math:`\gamma = 0`.

.. plot::

    import openturns as ot
    from matplotlib import pyplot as plt
    from openturns.viewer import View

    candidate = ot.Exponential(0.07, 0.0)
    graph = candidate.drawCDF(0.0, 30.0)

    sample = ot.Sample([[5.0], [6.0], [10.0], [22.0], [27.0]])
    empiricalDrawable = ot.UserDefined(sample).drawCDF(0.0, 30.0).getDrawable(0)
    empiricalDrawable.setColor('darkblue')
    graph.add(empiricalDrawable)

    graph.setTitle('CDF comparison')
    graph.setLegends(['Candidate CDF', 'Empirical CDF'])
    View(graph)


The test deals with the maximum deviation between the empirical distribution
and the candidate distribution, it is by nature highly sensitive to presence of
local deviations (a candidate distribution may be rejected even if it correctly
describes the sample for almost the whole domain of variation).

There is no rule to determine the minimum sample size one
needs to use this test; but it is often considered a reasonable approximation
when *N* is of an order of a few dozen. But whatever the value of *N*, the
distance -- and similarly the *p*-value -- remains a useful tool for comparing
different probability distributions to a sample. The distribution which minimizes
:math:`\widehat{D}_N` -- or maximizes the *p*-value -- will be of interest to the analyst.

This method is also referred to in the literature as Kolmogorov's Test.

.. topic:: API:

    - See :py:func:`~openturns.FittingTest_Kolmogorov` to compare a sample with
      a reference probability distribution
    - See :py:func:`~openturns.FittingTest_BestModelKolmogorov` to select the
      best candidate from several probability models with respect to a sample
    - See :py:func:`~openturns.HypothesisTest_TwoSamplesKolmogorov` to compare two samples

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

