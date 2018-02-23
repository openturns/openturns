.. _pcc:

Pearson Correlation Coefficient
-------------------------------

This method deals with the parametric modelling of a probability
distribution for a random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`. It aims to measure
a type of dependence (here a linear correlation) which may exist between
two components :math:`X^i` and :math:`X^j`.

The Pearson’s correlation coefficient :math:`\rho_{U,V}` aims to measure
the strength of a linear relationship between two random variables
:math:`U` and :math:`V`. It is defined as follows:

.. math::

   \begin{aligned}
       \rho_{U,V} = \frac{\displaystyle \Cov{U,V}}{\sigma_U \sigma_V}
     \end{aligned}

where
:math:`\Cov{U,V} = \Expect{ \left( U - m_U \right) \left( V - m_V \right) }`,
:math:`m_U= \Expect{U}`, :math:`m_V= \Expect{V}`,
:math:`\sigma_U= \sqrt{\Var{U}}` and :math:`\sigma_V= \sqrt{\Var{V}}`.
If we have a sample made up of a set of :math:`N` pairs
:math:`\left\{ (u_1,v_1),(u_2,v_2),\ldots,(u_N,v_N) \right\}`, Pearson’s
correlation coefficient can be estimated using the formula:

.. math::

   \begin{aligned}
       \widehat{\rho}_{U,V} = \frac{ \displaystyle \sum_{i=1}^N \left( u_i - \overline{u} \right) \left( v_i - \overline{v} \right) }{ \sqrt{\displaystyle \sum_{i=1}^N \left( u_i - \overline{u} \right)^2 \left( v_i - \overline{v} \right)^2} }
     \end{aligned}

where :math:`\overline{u}` and :math:`\overline{v}` represent the
empirical means of the samples :math:`(u_1,\ldots,u_N)` and
:math:`(v_1,\ldots,v_N)`.

Pearson’s correlation coefficient takes values between -1 and 1. The
closer its absolute value is to 1, the stronger the indication is that a
linear relationship exists between variables :math:`U` and :math:`V`.
The sign of Pearson’s coefficient indicates if the two variables
increase or decrease in the same direction (positive coefficient) or in
opposite directions (negative coefficient). We note that a correlation
coefficient equal to 0 does not necessarily imply the independence of
variables :math:`U` and :math:`V`: this property is in fact
theoretically guaranteed only if :math:`U` and :math:`V` both follow a
Normal distribution. In all other cases, there are two possible
situations in the event of a zero Pearson’s correlation coefficient:

-  the variables :math:`U` and :math:`V` are in fact independent,

-  or a non-linear relationship exists between :math:`U` and :math:`V`.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    N = 20
    ot.RandomGenerator.SetSeed(10)
    x = ot.Uniform(0.0, 10.0).getSample(N)
    f = ot.SymbolicFunction(['x'], ['5*x+10'])
    y = f(x) + ot.Normal(0.0, 5.0).getSample(N)
    graph = f.draw(0.0, 10.0)
    graph.setTitle('A linear relationship exists between U and V:\n Pearson\'s coefficient is a relevant measure of dependency')
    graph.setXTitle('u')
    graph.setYTitle('v')
    cloud = ot.Cloud(x, y)
    cloud.setPointStyle('circle')
    cloud.setColor('orange')
    graph.add(cloud)
    View(graph)

.. plot::

    import openturns as ot
    from openturns.viewer import View

    N = 20
    ot.RandomGenerator.SetSeed(10)
    x = ot.Uniform(0.0, 10.0).getSample(N)
    f = ot.SymbolicFunction(['x'], ['x^2'])
    y = f(x) + ot.Normal(0.0, 5.0).getSample(N)
    graph = f.draw(0.0, 10.0)
    graph.setTitle('There is a strong, non-linear relationship between U and V:\n Pearson\'s coefficient is not a relevant measure of dependency')
    graph.setXTitle('u')
    graph.setYTitle('v')
    cloud = ot.Cloud(x, y)
    cloud.setPointStyle('circle')
    cloud.setColor('orange')
    graph.add(cloud)
    View(graph)

.. plot::

    import openturns as ot
    from openturns.viewer import View

    N = 20
    ot.RandomGenerator.SetSeed(10)
    x = ot.Uniform(0.0, 10.0).getSample(N)
    f = ot.SymbolicFunction(['x'], ['5'])
    y = ot.Uniform(0.0, 10.0).getSample(N)
    graph = f.draw(0.0, 10.0)
    graph.setTitle('Pearson\'s coefficient estimate is quite close to zero\nbecause U and V are independent')
    graph.setXTitle('u')
    graph.setYTitle('v')
    cloud = ot.Cloud(x, y)
    cloud.setPointStyle('circle')
    cloud.setColor('orange')
    graph.add(cloud)
    View(graph)

.. plot::

    import openturns as ot
    from openturns.viewer import View

    N = 20
    ot.RandomGenerator.SetSeed(10)
    x = ot.Uniform(0.0, 10.0).getSample(N)
    f = ot.SymbolicFunction(['x'], ['30*sin(x)'])
    y = f(x) + ot.Normal(0.0, 5.0).getSample(N)
    graph = f.draw(0.0, 10.0)
    graph.setTitle('Pearson\'s coefficient estimate is quite close to zero\neven though U and V are not independent')
    graph.setXTitle('u')
    graph.setYTitle('v')
    cloud = ot.Cloud(x, y)
    cloud.setPointStyle('circle')
    cloud.setColor('orange')
    graph.add(cloud)
    View(graph)

The estimate :math:`\widehat{\rho}` of Pearson’s correlation
coefficient is sometimes denoted by :math:`r`.

.. topic:: API:

    - See :py:func:`~openturns.CorrelationAnalysis_PCC`

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/estimate_pcc`

.. topic:: References:

    - [Saporta1990]_
    - [Dixon1983]_
    - [NISThandbook]_
    - [Dagostino1986]_
    - [Bhattacharyya1997]_
    - [Sprent2001]_
    - [Burnham2002]_
