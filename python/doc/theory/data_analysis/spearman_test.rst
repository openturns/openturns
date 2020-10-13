.. _spearman_test:

Spearman correlation test
-------------------------

This method deals with the modelling of a probability distribution of a
random vector :math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`. It
seeks to find a type of dependency (here a monotonous correlation) which
may exist between two components :math:`X^i` and :math:`X^j`.

The Spearman’s correlation coefficient :math:`\rho^S_{U,V}`, defined in
:ref:`Spearman’s coefficient <spearman_coefficient>`
, measures the strength of a monotonous relationship between two random
variables :math:`U` and :math:`V`. If we have a sample made up of
:math:`N` pairs :math:`\left\{ (u_1,v_1),(u_2,v_2),(u_N,v_N) \right\}`,
we denote :math:`\widehat{\rho}^S_{U,V}` to be the estimated
coefficient.

Even in the case where two variables :math:`U` and :math:`V` have a
Spearman’s coefficient :math:`\rho^S_{U,V}` equal to zero, the estimate
:math:`\widehat{\rho}^S_{U,V}` obtained from the sample may be non-zero:
the limited sample size does not provide the perfect image of the real
correlation. Pearson’s test nevertheless enables one to determine if the
value obtained by :math:`\widehat{\rho}^S_{U,V}` is significantly
different from zero. More precisely, the user first chooses a
probability :math:`\alpha`. From this value the critical value
:math:`d_\alpha` is calculated automatically such that:

-  if :math:`\left| \widehat{\rho}^S_{U,V} \right| > d_\alpha`, one can
   conclude that the real Spearman’s correlation coefficient
   :math:`\rho^S_{U,V}` is not zero; the risk of error in making this
   assertion is controlled and equal to :math:`\alpha`;

-  if :math:`\left| \widehat{\rho}^S_{U,V} \right| \leq d_\alpha`, there
   is insufficient evidence to reject the null hypothesis
   :math:`\rho^S_{U,V} = 0`.

An important notion is the so-called “:math:`p`-value” of the test. This
quantity is equal to the limit error probability
:math:`\alpha_\textrm{lim}` under which the null correlation hypothesis
is rejected. Thus, Spearman’s’s coefficient is supposed non zero if and
only if :math:`\alpha_\textrm{lim}` is greater than the value
:math:`\alpha` desired by the user. Note that the higher
:math:`\alpha_\textrm{lim} - \alpha`, the more robust the decision.


.. plot::

    import openturns as ot
    from openturns.viewer import View

    N = 5
    ot.RandomGenerator.SetSeed(0)
    x = ot.Uniform(2.0, 8.0).getSample(N)
    f = ot.SymbolicFunction(['x'], ['80-0.4*(x-2)^3'])
    y = f(x) + ot.Normal(0.0, 20.0).getSample(N)
    graph = f.draw(2.0, 8.0)
    graph.setTitle('Non significant Spearman coefficient')
    graph.setXTitle('u')
    graph.setYTitle('v')
    cloud = ot.Cloud(x, y)
    cloud.setPointStyle('circle')
    cloud.setColor('orange')
    graph.add(cloud)
    View(graph)


.. topic:: API:

    - See :py:func:`~openturns.HypothesisTest_Spearman`
    - See :py:func:`~openturns.HypothesisTest_PartialSpearman`
    - See :py:func:`~openturns.HypothesisTest_FullSpearman`


.. topic:: Examples:

    - See :doc:`/auto_py_data_analysis/estimate_dependency_and_copulas/plot_independence_test`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
    - [nisthandbook]_
    - [dagostino1986]_
    - [bhattacharyya1997]_
    - [sprent2001]_
    - [burnham2002]_
