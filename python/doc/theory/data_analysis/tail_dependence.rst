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

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    copula = ot.FrankCopula()
    data = copula.getSample(1000)
    graph1 = ot.VisualTest.DrawUpperTailDependenceFunction(data)
    View(graph1)

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

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    copula = ot.FrankCopula()
    data = copula.getSample(1000)
    graph2 = ot.VisualTest.DrawUpperExtremalDependenceFunction(data)
    View(graph2)

**Lower tail dependence coefficient**

The lower tail dependence coefficient is defined by:

.. math::

    \lambda_L = \lim_{u \to 0} [F_2(X_2) < u, F1(X_1) < u]

The :math:`\lambda_L` coefficient gives the tendency for one variable to take extreme low values
knowing the other one is extreme low.

The variables :math:`(X_1,X_2)` are:

- asymptotically independent if :math:`\lambda_L=0`
- asymptotically dependent if :math:`0 < \lambda_L \leq 1`

Similarly to what is proposed for the upper tail coefficient we can define:

.. math::

    \chi_L(u) = \frac{\log 1-C(u,u)}{\log (1-u)}, \forall u \in [0,1]

When :math:`u` is close to :math:`0` then:

.. math::

    \chi_L(u) = \frac{C(u,u)}{u} + o(1) = \Pset[F_2(X_2) < u | F1(X_1) < u] + o(1)

which proves:

.. math::

    \lim_{u \to 0} \chi_L(u) = \lambda_L

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    copula = ot.FrankCopula()
    data = copula.getSample(1000)
    graph3 = ot.VisualTest.DrawLowerTailDependenceFunction(data)
    View(graph3)

**Lower extremal dependence coefficient**

Similarly we can introduce another function based on the comparison between the survival function
of the copula :math:`C` and the one assuming independence, defined by:

.. math::
    \begin{array}{lcl}
      \bar{\chi}_L(u) & = & \frac{\log (\Pset [F_1(X_1) > u] \Pset [F2(X_2) > u])}{\log \Pset [F_1(X_1) > u, F2(X_2) > u]} - 1\\
      & = & \frac{\log u^2}{\log \bar{C}(u,u)} - 1\\
      & = & \frac{2 \log u}{\log \bar{C}(u,u)} - 1
    \end{array}

We define the lower extremal coefficient :math:`\bar{\chi}_L` by:

.. math::

    \bar{\chi}_L = \lim_{u \to 0} \bar{\chi}_L(u)

We have :math:`-1 \leq \bar{\chi}_L \leq 1`.
We show that :math:`\lambda_L \neq 0` only if :math:`\bar{\chi}_L = 1`.
For asymptotically independent variables (:math:`\chi_L = 0`), :math:`\bar{\chi}_L` gives the strength of the vanishing dependency.

We define the following rules:

- if :math:`\lambda_L > 0` (and :math:`\bar{\chi}_L = 1`): the variables are asymptotically dependent in the low values
  and :math:`\bar{\chi}_L` gives a measure of the strength of the dependence.
- if :math:`\lambda_L = 0`: the variables are asymptotically independent in the low values
  and :math:`\bar{\chi}_L` gives the strength of the vanishing dependency.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    copula = ot.FrankCopula()
    data = copula.getSample(1000)
    graph4 = ot.VisualTest.DrawLowerExtremalDependenceFunction(data)
    View(graph4)

.. topic:: API:

    - See :py:func:`~openturns.VisualTest.DrawUpperTailDependenceFunction`
    - See :py:func:`~openturns.VisualTest.DrawUpperExtremalDependenceFunction`
    - See :py:func:`~openturns.VisualTest.DrawLowerTailDependenceFunction`
    - See :py:func:`~openturns.VisualTest.DrawLowerExtremalDependenceFunction`
    - See :py:func:`~openturns.Distribution.drawUpperTailDependenceFunction`
    - See :py:func:`~openturns.Distribution.drawUpperExtremalDependenceFunction`
    - See :py:func:`~openturns.Distribution.drawLowerTailDependenceFunction`
    - See :py:func:`~openturns.Distribution.drawLowerExtremalDependenceFunction`

.. topic:: References:

    - [beirlant2004]_
