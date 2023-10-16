.. _tail_dependence:

Tail dependence coefficients
----------------------------

The tail dependence coefficients helps to assess the asymptotic dependency of a bivariate random variables. We
detail here the following ones:

- the upper tail dependence coefficient denoted by :math:`\lambda_U` (which is the notation of the probability
  community) as well as :math:`\chi` (which is the notation of the extreme value community),
- the upper extremal dependence coefficient denoted by :math:`\bar{\chi}`,
- the lower tail dependence coefficient denoted by :math:`\lambda_L`,
- the upper extremal dependence coefficient denoted by :math:`\bar{\chi}_L`.

Readers should refer to [beirlant2004]_ to get more details.

Let :math:`\vect{X} = (X_1, X_2)` be a bivariate random vector with marginal distribution functions
:math:`F_1` and :math:`F_2`, and copula :math:`C`.

**Upper tail dependence coefficient**

We denote by :math:`\lambda_U` or :math:`\chi` the upper tail dependence coefficient:

.. math::

    \lambda_U = \chi = \lim_{u \to 1} \Pset[F_2(X_2) > u | F_1(X_1) > u]

provided that the limit exists.

The :math:`\chi` coefficient can be interpreted as the tendency for one variable to take extreme high values
given that the other variable is extremely high.

The variables :math:`(X_1, X_2)` are said to be:

- asymptotically independent if and only if :math:`\chi=0`,
- asymptotically dependent if and only if :math:`0 <\chi \leq 1`.

Now, we define the function :math:`\chi(u)` defined by:

.. math::

    \chi(u) = 2 - \frac{\log C(u,u)}{\log u}, \forall u \in [0,1]

We can see the tail dependence coefficient as the limit of the function :math:`\chi(u)` when :math:`u` tends
to :math:`1`. As a matter of fact, when :math:`u` is close to :math:`1`, we have:

.. math::

    \chi(u) = 2 - \frac{1-C(u,u)}{1-u} + o(1) = \Pset[F_2(X_2) > u | F1(X_1) > u] + o(1)

which proves that:

.. math::

    \lim_{u \to 1} \chi(u) = \lambda_U = \chi

Next to providing the limit :math:`\chi`, the function :math:`\chi(u)` also provides some insight in the
dependence structure of the variables at lower quantile levels: the larger :math:`|\chi(u)|` the more
correlated are the variables.

The function :math:`\chi(u)` is estimated on data for several :math:`u` which must not be too high because
of a problem of lack of data greater than :math:`u`. Condifence intervals can be estimated on each
:math:`\chi(u)`. If the confidence interval contains the zero value when :math:`u` tends to :math:`1`, then we can assume that :math:`\chi=0`.

Within the class of asymptotically dependent variables, the value of  :math:`\chi` increases with increasing degree of dependence at extreme levels.

We illustrate two cases where the variables are:

- asymptotically independent: we generated a sample of size :math:`10^3` from a Frank copula which has a
  zero upper tail coefficient whatever the parameter,
- asymptotically dependent: we generated a sample of size :math:`10^3` from a Gumbel copula parametrized
  by :math:`\theta = 2.0` which has a positive upper tail coefficient.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    copula1 = ot.FrankCopula()
    data1 = copula1.getSample(1000)
    copula2 = ot.GumbelCopula(2.0)
    data2 = copula2.getSample(1000)
    graph1 = ot.VisualTest.DrawUpperTailDependenceFunction(data1)
    graph1.setTitle('Upper tail dependence function: Frank')
    graph2 = ot.VisualTest.DrawUpperTailDependenceFunction(data2)
    graph2.setTitle('Upper tail dependence function: Gumbel(2.0)')
    grid = ot.GridLayout(1,2)
    grid.setGraph(0,0,graph1)
    grid.setGraph(0,1,graph2)
    View(grid)

**Upper extremal dependence coefficient**

Within the class of asymptotically independent variables, the degrees of relative strength of dependence is
given by the function :math:`\chi(u)` defined by:

.. math::

    \bar{\chi}(u) = \frac{\log (\Pset [F_1(X_1) > u] \Pset [F_2(X_2) > u])}{\log [F_1(X_1) > u, F_2(X_2) > u]}, \forall u \in [0,1]

We show that:

.. math::

    \bar{\chi}(u) = \frac{2 \log 1-u}{\log \bar{C}(u,u)} - 1, \forall u \in [0,1]

where :math:`\bar{C}` is the copula survivor function defined by:

.. math::

    \bar{C}(u_1, u_2) =  \Pset [U_1 > u_1, U_2 > u_2] = 1-u_1-u_2+C(u_1, u_2), \forall u \in [0,1]

And we can define the upper extremal dependence coefficient by:

.. math::

    \bar{\chi} = \lim_{u \to 1} \bar{\chi}(u)

We show that :math:`-1 \leq \bar{\chi} \leq 1` and that if the variables are asymptotically dependent, then :math:`\bar{\chi} =1`:

.. math::

    \chi > 0 \Rightarrow \lim_{u \to 1} \bar{\chi}(u) = 1

We illustrate the function :math:`\bar{\chi}(u)` for both previous cases.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    copula1 = ot.FrankCopula()
    data1 = copula1.getSample(1000)
    copula2 = ot.GumbelCopula(2.0)
    data2 = copula2.getSample(1000)
    graph1 = ot.VisualTest.DrawUpperExtremalDependenceFunction(data1)
    graph1.setTitle('Upper extremal dependence function: Frank')
    graph2 = ot.VisualTest.DrawUpperExtremalDependenceFunction(data2)
    graph2.setTitle('Upper extremal dependence function: Gumbel(2.0)')
    grid2 = ot.GridLayout(1,2)
    grid2.setGraph(0,0,graph1)
    grid2.setGraph(0,1,graph2)
    View(grid2)

As a result, the pair :math:`(\chi, \bar{\chi})` can be used as a summary of extremal dependence of
:math:`\vect{X} = (X_1, X_2)` as follows:

- if :math:`0 < \chi \leq 1` (and then :math:`\bar{\chi}=1`), then :math:`X_1` and :math:`X_2` are
  asymptotically dependent in extreme high values and :math:`\chi` is a measure for strength of dependence,
- if :math:`\chi = 0` and :math:`-1 \leq \bar{\chi} < 1`, then :math:`X_1` and :math:`X_2` are asymptotically
  independent in extreme high values and :math:`\bar{\chi}` is a measure for strength of dependence. If
  :math:`\bar{\chi} >0`, there is a positive association: simultanueous extreme high values occur more frequently
  than under exact independence. If :math:`\bar{\chi} <0`, there is a negative association: simultanueous
  extreme high values occur less frequently than under exact independence.

**Lower tail dependence coefficient**

We denote by :math:`\lambda_L` the lower tail dependence coefficient:

.. math::

    \lambda_L = \lim_{u \to 0} [F_2(X_2) < u| F_1(X_1) < u]

provided that the limit exists.

The :math:`\lambda_L` coefficient can be interpreted as the tendency for one variable to take extreme low values
given that the other variable is extremely low.

The variables :math:`(X_1, X_2)` are said to be:

- asymptotically independent if and only if :math:`\lambda_L=0`,
- asymptotically dependent if and only if :math:`0 < \lambda_L \leq 1`.

Similarly to what is proposed for the upper tail coefficient, we can define the function :math:`\chi_L(u)` by:

.. math::

    \chi_L(u) = \frac{\log (1 - C(u,u))}{\log (1-u)}, \forall u \in [0,1]



We can see the tail dependence coefficient as the limit of the function :math:`\chi(u)` when :math:`u` tends
to :math:`0`. As a matter of fact, when :math:`u` is close to :math:`0`, we have:

.. math::

    \chi_L(u) = \frac{C(u,u)}{u} + o(1) = \Pset[F_2(X_2) < u | F_1(X_1) < u] + o(1)

which proves that:

.. math::

    \lim_{u \to 0} \chi_L(u) = \lambda_L

We show that :math:`0 \leq \chi_L(u) \leq 1`.

Next to providing the limit :math:`\lambda_L`, the function :math:`\chi_L(u)` also provides some insight in
the dependence structure of the variables at upper quantile levels: The larger :math:`|\chi_L(u)|` the more
correlated are the variables.

The function :math:`\chi_L(u)` is estimated on data for several :math:`u` which must not be too low because
of a problem of lack of data lesser than :math:`u`. Condifence intervals can be estimated on each
:math:`\chi_L(u)`. If the confidence interval contains the zero value when :math:`u` tends to :math:`0`, then
we can assume that :math:`\lambda_L=0`.

Within the class of asymptotically dependent variables, the value of  :math:`\chi_L` increases with increasing
degree of dependence at extreme levels.

We illustrate two cases where the variables are:

- asymptotically independent: we generated a sample of size :math:`10^3` from a Frank copula which has a zero
  lower tail coefficient whatever the parameter,
- asymptotically dependent: we generated a sample of size :math:`10^3` from a Clayton copula parametrized by
  :math:`\theta = 2.0` which has a positive lower tail coefficient.


.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    copula1 = ot.FrankCopula()
    data1 = copula1.getSample(1000)
    copula2 = ot.ClaytonCopula(2.0)
    data2 = copula2.getSample(1000)
    graph1 = ot.VisualTest.DrawLowerTailDependenceFunction(data1)
    graph1.setTitle('Lower tail dependence function:Frank :')
    graph2 = ot.VisualTest.DrawLowerTailDependenceFunction(data2)
    graph2.setTitle('Lower tail dependence function: Gumbel(2.0)')
    grid3 = ot.GridLayout(1,2)
    grid3.setGraph(0,0,graph1)
    grid3.setGraph(0,1,graph2)
    View(grid3)

**Lower extremal dependence coefficient**

Within the class of asymptotically independent variables, the degrees of relative strength of dependence is
given by the function :math:`\chi_L(u)` defined by:

.. math::

      \bar{\chi}_L(u) = \frac{\log (\Pset [F_1(X_1) < u] \Pset [F_2(X_2) < u])}{\log \Pset [F_1(X_1) < u, F_2(X_2) < u]} - 1, \forall u \in [0,1]

We show that:

.. math::

    \bar{\chi}_L(u) = \frac{2 \log u}{\log C(u,u)} - 1, \forall u \in [0,1]


And we can define the lower extremal dependence coefficient by:

.. math::

    \bar{\chi}_L = \lim_{u \to 0} \bar{\chi}_L(u)

We show that :math:`-1 \leq \bar{\chi}_L \leq 1` and that if the variables are asymptotically dependent, then :math:`\bar{\chi}_L=1`:

.. math::

    \lambda_L > 0 \Rightarrow \lim_{u \to 0} \bar{\chi}_L(u) = 1

We illustrate the function :math:`\bar{\chi}_L(u)` for both previous cases: the Frank copula
:math:`\bar{\chi}(u)` function is on the left and the Clayton copula :math:`\bar{\chi}(u)` function is on
the right.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    ot.RandomGenerator.SetSeed(0)
    copula1 = ot.FrankCopula()
    data1 = copula1.getSample(1000)
    copula2 = ot.ClaytonCopula(2.0)
    data2 = copula2.getSample(1000)
    graph1 = ot.VisualTest.DrawLowerExtremalDependenceFunction(data1)
    graph1.setTitle('Lower extremal dependence function for the Frank copula')
    graph2 = ot.VisualTest.DrawLowerExtremalDependenceFunction(data2)
    graph2.setTitle('Lower extremal dependence function for the Clayton(2.0) copula')
    grid4 = ot.GridLayout(1,2)
    grid4.setGraph(0,0,graph1)
    grid4.setGraph(0,1,graph2)
    View(grid4)

As a result, the pair :math:`(\chi_L, \bar{\chi}_L)` can be used as a summary of extremal dependence of
:math:`\vect{X} = (X_1, X_2)` as follows:

- if :math:`0 < \chi_L \leq 1` (and then :math:`\bar{\chi}_L=1`), then :math:`X_1` and :math:`X_2` are
  asymptotically dependent in extreme low values and :math:`\chi` is a measure for strength of dependence,
- if :math:`\chi_L = 0` and :math:`-1 \leq \bar{\chi}_L < 1`, then :math:`X_1` and :math:`X_2` are
  asymptotically independent in extreme low values and :math:`\bar{\chi}` is a measure for strength of dependence.
  If :math:`\bar{\chi}_L >0`, there is a positive association: simultaneous extreme low values occur more
  frequently than under exact independence. If :math:`\bar{\chi}_L <0`, there is a negative association:
  simultaneous extreme low values occur less frequently than under exact independence.

.. topic:: API:

    - See :py:func:`~openturns.VisualTest.DrawUpperTailDependenceFunction`
    - See :py:func:`~openturns.VisualTest.DrawUpperExtremalDependenceFunction`
    - See :py:func:`~openturns.VisualTest.DrawLowerTailDependenceFunction`
    - See :py:func:`~openturns.VisualTest.DrawLowerExtremalDependenceFunction`
    - See :py:func:`~openturns.Distribution.drawUpperTailDependenceFunction`
    - See :py:func:`~openturns.Distribution.drawUpperExtremalDependenceFunction`
    - See :py:func:`~openturns.Distribution.drawLowerTailDependenceFunction`
    - See :py:func:`~openturns.Distribution.drawLowerExtremalDependenceFunction`
    - See :py:func:`~openturns.Distribution.computeUpperTailDependenceMatrix`
    - See :py:func:`~openturns.Distribution.computeExtremalDependenceMatrix`
    - See :py:func:`~openturns.Distribution.computeLowerTailDependenceMatrix`
    - See :py:func:`~openturns.Distribution.computeLowerExtremalDependenceMatrix`

.. topic:: References:

    - [beirlant2004]_
