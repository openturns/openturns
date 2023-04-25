.. _tail_dependence:

Tail dependence coefficients
----------------------------

The tail dependence coefficients helps to assess the asymptotic dependency of a bivariate random variables.
Let :math:`X` a bivariate random variable which components :math:`X_i` follow the
cumulative distribution fonctions :math:`F_i` and copula :math:`C`.

**Upper tail dependence coefficient**

We note :math:`\chi` the upper tail dependence coefficient:

.. math::

    \lambda_U = \chi = \lim_{u \to 1} \Pset[F_2(X_2) > u | F1(X_1) > u]

The :math:`\chi` coefficient represents the tendency for one variable to take extreme high values
knowing the other variable is extremely high.

The variables :math:`(X_1,X_2)` are:

- asymptotically independent if :math:`\chi=0`
- asymptotically dependent if :math:`0 \leq \chi \leq 1`

Let the function :math:`\chi(u)` defined by:

.. math::

    \chi(u) = 2 - \frac{\log C(u,u)}{\log u}, \forall u \in [0,1]

When :math:`u` is close to :math:`1` then:

.. math::

    \chi(u) = 2 - \frac{1-C(u,u)}{1-u} + o(1) = \Pset[F_2(X_2) > u | F1(X_1) > u] + o(1)

which proves:

.. math::

    \lim_{u \to 1} \chi(u) = \lambda_U = \chi

The :math:`\chi(u)` function also gives information on the dependence structure between the variables
at quantiles levels less than :math:`1`.

**Upper extremal dependence coefficient**

Let the function :math:`\chi(u)` defined by:

.. math::

    \bar{\chi}(u) = \frac{\log (\Pset [F_1(X_1) > u] \Pset [F2(X_2) > u])}{\log [F_1(X_1) > u, F2(X_2) > u]}, \forall u \in [0,1]

We show that:

.. math::

    \bar{\chi}(u) = \frac{2 \log 1-u}{\log C(u,u)} - 1, \forall u \in [0,1]

And we define the asymtotic independence coefficient

.. math::

    \bar{\chi} = \lim_{u \to 1} \bar{\chi}(u)

The variables :math:`(X_1,X_2)` are:

- asymptotically dependent if :math:`\bar{\chi}=1`
- asymptotically independent if :math:`-1 \leq \bar{\chi} \leq 1`


**Lower tail dependence coefficient**

**Lower extremal dependence coefficient**

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    data = ot.FrankCopula().getSample(1000)
    graph1 = ot.VisualTest.DrawUpperTailDependenceFunction(data)
    graph2 = ot.VisualTest.DrawUpperExtremalDependenceFunction(data)
    graph3 = ot.VisualTest.DrawLowerTailDependenceFunction(data)
    graph4 = ot.VisualTest.DrawLowerExtremalDependenceFunction(data)
    View(graph1)
    View(graph2)
    View(graph3)
    View(graph4)

.. topic:: API:

    - See :py:func:`~openturns.VisualTest.DrawUpperTailDependenceFunction`
    - See :py:func:`~openturns.VisualTest.DrawUpperExtremalDependenceFunction`
    - See :py:func:`~openturns.VisualTest.DrawLowerTailDependenceFunction`
    - See :py:func:`~openturns.VisualTest.DrawLowerExtremalDependenceFunction`

.. topic:: References:

    - [beirlant2004]_
