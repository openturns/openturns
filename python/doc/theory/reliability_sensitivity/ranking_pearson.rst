Uncertainty ranking: Pearson's correlation
------------------------------------------

This method deals with analysing the influence the random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)` has on a random
variable :math:`Y^j` which is being studied for uncertainty. Here we
attempt to measure linear relationships that exist between :math:`Y^j`
and the different components :math:`X^i`.

Pearson’s correlation coefficient :math:`\rho_{Y^j,X^i}`, defined in ,
measures the strength of a linear relation between two random variables
:math:`Y^j` and :math:`X^i`. If we have a sample made up of :math:`N`
pairs :math:`(y^j_1,x^i_1)`, :math:`(y^j_2,x^i_2)`, …,
:math:`(y^j_N,x^i_N)`, we can obtain :math:`\widehat{\rho}_{Y^j,X^i}` an
estimation of Pearson’s coefficient. The hierarchical ordering of
Pearson’s coefficients is of interest in the case where the relationship
between :math:`Y^j` and :math:`n_X` variables
:math:`\left\{ X^1,\ldots,X^{n_X} \right\}` is close to being a linear
relation:

.. math::

   \begin{aligned}
       Y^j \simeq a_0 + \sum_{i=1}^{n_X} a_i X^i
     \end{aligned}

To obtain an indication of the role played by each :math:`X^i` in the
dispersion of :math:`Y^j`, the idea is to estimate Pearson’s correlation
coefficient :math:`\widehat{\rho}_{X^i,Y^j}` for each :math:`i`. One can
then order the :math:`n_X` variables :math:`X^1,\ldots, X^{n_X}` taking
absolute values of the correlation coefficients: the higher the value of
:math:`\left| \widehat{\rho}_{X^i,Y^j} \right|` the greater the impact
the variable :math:`X^i` has on the dispersion of :math:`Y^j`.


.. plot::

    import openturns as ot
    from matplotlib import pyplot as plt
    from openturns.viewer import View

    # Data
    size = 5
    x1 = ot.Uniform(1.0, 9.0).getSample(size)
    y1 = ot.Uniform(0.0, 120.0).getSample(size)
    size = 50 - size
    x2 = ot.Uniform(1.0, 9.0).getSample(size)
    y2 = ot.Uniform(0.0, 120.0).getSample(size)

    # Merge with previous data
    x = ot.Sample(x1)
    y = ot.Sample(y1)
    x.add(x2)
    y.add(y2)

    # Linear model
    algo = ot.LinearLeastSquares(x, y)
    algo.run()
    linear = algo.getMetaModel()

    graph = ot.Graph('Null Pearson coefficient', 'u', 'v', True, '')
    cloud1 = ot.Cloud(x1, y1)
    cloud1.setPointStyle('diamond')
    cloud1.setColor("orange")
    cloud1.setLineWidth(2)
    graph.add(cloud1)
    cloud2 = ot.Cloud(x2, y2)
    cloud2.setPointStyle('square')
    cloud2.setColor("blue")
    cloud2.setLineWidth(2)
    graph.add(cloud2)
    curve2 = ot.Curve(x, linear(x))
    curve2.setColor("black")
    curve2.setLineWidth(2)
    graph.add(curve2)
    View(graph)


.. topic:: API:

    - See :py:meth:`~openturns.HypothesisTest_Pearson`


.. topic:: Examples:

    - See :doc:`/examples/data_analysis/independence_test`


.. topic:: References:

    - Saltelli, A., Chan, K., Scott, M. (2000). "Sensitivity Analysis", John Wiley \& Sons publishers, Probability and Statistics series
    - J.C. Helton, F.J. Davis (2003). "Latin Hypercube sampling and the propagation of uncertainty analyses of complex systems". Reliability Engineering and System Safety 81, p.23-69
    - J.P.C. Kleijnen, J.C. Helton (1999). "Statistical analyses of scatterplots to identify factors in large-scale simulations, part 1 : review and comparison of techniques". Reliability Engineering and System Safety 65, p.147-185

