Uncertainty ranking: Spearman's correlation
-------------------------------------------

This method deals with analyzing the influence the random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)` has on a random
variable :math:`Y^j` which is being studied for uncertainty. Here we
attempt to measure monotonic relationships that exist between
:math:`Y^j` and the different components :math:`X^i`.

Spearman’s correlation coefficient :math:`\rho^S_{Y^j,X^i}`, defined in
, measures the strength of a monotonic relation between two random
variables :math:`Y^j` and :math:`X^i`. If we have a sample made up of
:math:`N` pairs :math:`(y^j_1,x^i_1)`, :math:`(y^j_2,x^i_2)`, …,
:math:`(y^j_N,x^i_N)`, we can obtain :math:`\widehat{\rho}^S_{Y^j,X^i}`
an estimation of Spearman’s coefficient.

Hierarchical ordering using Spearman’s coefficients deals with the case
where the variable :math:`Y^j` monotonically depends on the :math:`n_X`
variables :math:`\left\{ X^1,\ldots,X^{n_X} \right\}`. To obtain an
indication of the role played by each :math:`X^i` in the dispersion of
:math:`Y^j`, the idea is to estimate the Spearman correlation
coefficients :math:`\widehat{\rho}^S_{X^i,Y^j}` for each :math:`i`. One
can then order the :math:`n_X` variables :math:`X^1,\ldots, X^{n_X}`
taking absolute values of the Spearman coefficients: the higher the
value of :math:`\left| \widehat{\rho}^S_{X^i,Y^j} \right|`, the greater
the impact the variable :math:`X^i` has on the dispersion of
:math:`Y^j`.


.. plot::

    import openturns as ot
    from matplotlib import pyplot as plt
    from openturns.viewer import View
    size = 6
    ot.RandomGenerator.SetSeed(3)

    # Data
    x1 = ot.Uniform(1.0, 9.0).getSample(size)
    y1 = ot.Uniform(0.0, 120.0).getSample(size)
    graph = ot.Graph("Non significant Spearman coefficient", "u", "v", True, "")
    cloud1 = ot.Cloud(x1, y1)
    cloud1.setPointStyle("diamond")
    cloud1.setColor("orange")
    cloud1.setLineWidth(2)
    graph.add(cloud1)

    size = 150 - size

    # Data
    x2 = ot.Uniform(1.0, 9.0).getSample(size)
    y2 = ot.Uniform(0.0, 120.0).getSample(size)
    # Merge with previous data
    x = ot.Sample(x1)
    y = ot.Sample(y1)
    x.add(x2)
    y.add(y2)
    # Quadratic model
    algo = ot.QuadraticLeastSquares(x, y)
    algo.run()
    quadratic = algo.getMetaModel()

    graph = ot.Graph("Null Spearman coefficient", "u", "v", True, "")
    graph.add(cloud1)
    cloud2 = ot.Cloud(x2, y2)
    cloud2.setPointStyle("square")
    cloud2.setColor("blue")
    cloud2.setLineWidth(2)
    graph.add(cloud2)
    curve2 = ot.Curve(x, quadratic(x))
    curve2.setColor("black")
    curve2.setLineWidth(2)
    graph.add(curve2)

    View(graph)

.. topic:: API:

    - See :class:`~openturns.HypothesisTest_Spearman`


.. topic:: Examples:

    - See :doc:`/examples/data_analysis/independence_test`


.. topic:: References:

    - Saltelli, A., Chan, K., Scott, M. (2000). "Sensitivity Analysis", John Wiley \& Sons publishers, Probability and Statistics series
    - J.C. Helton, F.J. Davis (2003). "Latin Hypercube sampling and the propagation of uncertainty analyses of complex systems". Reliability Engineering and System Safety 81, p.23-69
    - J.P.C. Kleijnen, J.C. Helton (1999). "Statistical analyses of scatterplots to identify factors in large-scale simulations, part 1 : review and comparison of techniques". Reliability Engineering and System Safety 65, p.147-185

