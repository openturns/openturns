.. _chi2_independence_test:

Chi-squared test for independence
---------------------------------

This method deals with the parametric modelling of a probability
distribution for a random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`. We seek here to
detect possible dependencies that may exist between two components
:math:`X^i` and :math:`X^j`. The :math:`\chi^2` test for Independence
for discrete probability distributions can be used.

As we are considering discrete distributions, the possible values for
:math:`X^i` and :math:`X^j` respectively belong to the discrete sets
:math:`\cE_i` and :math:`\cE_j`. The :math:`\chi^2` test of independence
can be applied when we have a sample consisting of :math:`N` pairs
:math:`\left\{ (x^i_1,x^j_1),(x^i_2,x^j_2),(x^i_N,x^j_N) \right\}`. We
denote:

-  :math:`n_{u,v}` the number of pairs in the sample such that
   :math:`x^i_k = u` and :math:`x^j_k = v`,

-  :math:`n^i_{u}` the number of pairs in the sample such that
   :math:`x^i_k = u`,

-  :math:`n^j_{v}` the number of pairs in the sample such that
   :math:`x^j_k = v`.

The test thus uses the quantity denoted :math:`\widehat{D}_N^2`:

.. math::

   \begin{aligned}
       \widehat{D}_N^2 = \sum_{u \in \cE_i}\sum_{v\in \cE_2}\frac{\left(p_{u,v} - p^j_{v}p^i_{u}\right)^2}{p^i_{u}p^j_{v}}
     \end{aligned}

where:

.. math::

   \begin{aligned}
       p_{u,v} = \frac{n_{u,v}}{N},\ p^i_{u} =  \frac{n^i_{u}}{N},\ p^j_{v} =  \frac{n^j_{v}}{N}
     \end{aligned}

The probability distribution of the distance :math:`\widehat{D}_N^2` is
asymptotically known (i.e. as the size of the sample tends to infinity).
If :math:`N` is sufficiently large, this means that for a probability
:math:`\alpha`, one can calculate the threshold (critical value)
:math:`d_\alpha` such that:

-  if :math:`\widehat{D}_N>d_{\alpha}`, we conclude, with the risk of
   error :math:`\alpha`, that a dependency exists between :math:`X^i`
   and :math:`X^j`,

-  if :math:`\widehat{D}_N \leq d_{\alpha}`, the independence hypothesis
   is considered acceptable.

An important notion is the so-called “:math:`p`-value” of the test. This
quantity is equal to the limit error probability
:math:`\alpha_\textrm{lim}` under which the independence hypothesis is
rejected. Thus, independence is assumed if and only if
:math:`\alpha_\textrm{lim}` is greater than the value :math:`\alpha`
desired by the user. Note that the higher
:math:`\alpha_\textrm{lim} - \alpha`, the more robust the decision.

This method is also referred to in the literature as the :math:`\chi^2`
test of contingency.


.. topic:: API:

    - See :class:`~openturns.HypothesisTest_ChiSquared`


.. topic:: Examples:

    - See :doc:`/examples/data_analysis/independence_test`


.. topic:: References:

    - [Saporta1990]_
    - [Dixon1983]_
    - [NISThandbook]_
    - [Dagostino1986]_
    - [Bhattacharyya1997]_
    - [Sprent2001]_
    - [Burnham2002]_
