.. _smirnov_test:

Kolmogorov-Smirnov two samples test
-----------------------------------

Let :math:`X` be a scalar uncertain variable modeled as a random
variable. This method deals with the construction of a dataset prior to
the choice of a probability distribution for :math:`X`. This statatistical
test is used to compare two samples :math:`\left\{x_1,\ldots,x_N \right\}`
and :math:`\left\{x'_1,\ldots,x'_M \right\}`; the goal is to determine
whether these two samples come from the same probability distribution or
not. If this is the case, the two samples should be aggregated in order
to increase the robustness of further statistical analysis.

The test relies on the maximum distance between the cumulative distribution
functions :math:`\widehat{F}_N` and :math:`\widehat{F}'_M` of the samples
:math:`\left\{x_1,\ldots,x_N \right\}` and :math:`\left\{x'_1,\ldots,x'_M \right\}`.
This distance is expressed as follows:

.. math::

   \begin{aligned}
       \widehat{D}_{M,N} = \sup_x \left|\widehat{F}_N\left(x\right) - \widehat{F}'_M\left(x\right)\right|
     \end{aligned}

The probability distribution of the distance :math:`\widehat{D}_{M,N}`
is asymptotically known (i.e. as the size of the samples tends to
infinity). If :math:`M` and :math:`N` are sufficiently large, this means
that for a probability :math:`\alpha`, one can calculate the threshold /
critical value :math:`d_\alpha` such that:

-  if :math:`\widehat{D}_{M,N} >d_{\alpha}`, we conclude that the two
   samples are not identically distributed, with a risk of error
   :math:`\alpha`,

-  if :math:`\widehat{D}_{M,N} \leq d_{\alpha}`, it is reasonable to say
   that both samples arise from the same distribution.

An important notion is the so-called “:math:`p`-value” of the test. This
quantity is equal to the limit error probability
:math:`\alpha_\textrm{lim}` under which the “identically-distributed”
hypothesis is rejected. Thus, the two samples will be supposed
identically distributed if and only if :math:`\alpha_\textrm{lim}` is
greater than the value :math:`\alpha` desired by the user. Note that the
higher :math:`\alpha_\textrm{lim} - \alpha`, the more robust the
decision.

This test is also referred to as the Kolmogorov-Smirnov’s test for two
samples.

.. topic:: API:

    - See :py:func:`~openturns.HypothesisTest_TwoSamplesKolmogorov`

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/smirnov_test`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
