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

The probability distribution of the distance :math:`\widehat{D}_N` is
asymptotically known (i.e. as the size of the sample tends to infinity).
If *N* is sufficiently large, this means that for a probability
:math:`\alpha` and a candidate distribution type, one can compute the
threshold / critical value :math:`d_\alpha` such that:

- if  :math:`\widehat{D}_N>d_{\alpha}`, we reject the candidate distribution
  with a risk of error :math:`\alpha`,
- if  :math:`\widehat{D}_N \leq d_{\alpha}`, the candidate distribution is
  considered acceptable.

Note that :math:`d_\alpha` does not depend on the candidate distribution
*F* being tested, and the test is therefore relevant for any continuous
distribution.

An important notion is the so-called *p*-value of the test. This quantity is
equal to the limit error probability :math:`\alpha_\textrm{lim}` under which
the candidate distribution is rejected.
Thus, the candidate distribution will be accepted if and only if
:math:`\alpha_\textrm{lim}` is greater than the value :math:`\alpha` desired by
the user.
Note that the higher :math:`\alpha_\textrm{lim} - \alpha`, the more robust the decision.

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
    empiricalDrawable = ot.VisualTest.DrawEmpiricalCDF(sample, 0.0, 30.0).getDrawable(0)
    empiricalDrawable.setColor('darkblue')
    graph.add(empiricalDrawable)

    graph.setTitle('CDF comparison')
    graph.setLegends(['Candidate CDF', 'Empirical CDF'])
    View(graph)


The test deals with the maximum deviation between the empirical distribution
and the candidate distribution, it is by nature highly sensitive to presence of
local deviations (a candidate distribution may be rejected even if it correctly
describes the sample for almost the whole domain of variation).

We remind the reader that the underlying theoretical results of the test are
asymptotic. There is no rule to determine the minimum number of data values one
needs to use this test; but it is often considered a reasonable approximation
when *N* is of an order of a few dozen. But whatever the value of *N*, the
distance -- and similarly the *p*-value -- remains a useful tool for comparing
different probability distributions to a sample. The distribution which minimizes
:math:`\widehat{D}_N` -- or maximizes the *p*-value -- will be of interest to the analyst.

We also point out that the calculation of :math:`d_\alpha` should in theory be
modified if on is testing the goodness-of-fit to a parametric model where the
parameters have been estimated from the same sample. The current version does
not allow this modification, and the results should be therefore used with
caution when the *p*-value :math:`\alpha_\textrm{lim}` and the desired error
risk :math:`\alpha` are very close.

This method is also referred to in the literature as Kolmogorov's Test.

.. topic:: API:

    - See :py:func:`~openturns.FittingTest_Kolmogorov` to compare a sample with
      a reference probability distribution
    - See :py:func:`~openturns.FittingTest_BestModelKolmogorov` to select the
      best candidate from several probability models with respect to a sample
    - See :py:func:`~openturns.FittingTest_TwoSamplesKolmogorov` to compare two samples

.. topic:: Examples:

    - See :ref:`examples/data_analysis/kolmogorov_test.ipynb`

.. topic:: References:

    - [Saporta1990]_
    - [Dixon1983]_
    - [NISThandbook]_
    - [Dagostino1986]_
    - [Sprent2001]_
    - [Bhattacharyya1997]_
