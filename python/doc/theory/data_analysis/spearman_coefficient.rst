.. _spearman_coefficient:

Spearman correlation coefficient
--------------------------------

This method deals with the parametric modelling of a probability
distribution for a random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`. It aims to measure
a type of dependence (here a monotonous correlation) which may exist
between two components :math:`X^i` and :math:`X^j`.

The Spearman’s correlation coefficient :math:`\rho^S_{U,V}` aims to
measure the strength of a monotonic relationship between two random
variables :math:`U` and :math:`V`. It is in fact equivalent to the
Pearson’s correlation coefficient after having transformed :math:`U` and
:math:`V` to linearize any monotonic relationship (remember that
Pearson’s correlation coefficient may only be used to measure the
strength of linear relationships, see :ref:`Pearson’s correlation coefficient <pcc>`):

.. math::

   \begin{aligned}
       \rho^S_{U,V} = \rho_{F_U(U),F_V(V)}
     \end{aligned}

where :math:`F_U` and :math:`F_V` denote the cumulative distribution
functions of :math:`U` and :math:`V`.

If we arrange a sample made up of :math:`N` pairs
:math:`\left\{ (u_1,v_1),(u_2,v_2),\ldots,(u_N,v_N) \right\}`, the
estimation of Spearman’s correlation coefficient first of all requires a
ranking to produce two samples :math:`(u_1,\ldots,u_N)` and
:math:`(v_1,\ldots,v_N)`. The ranking :math:`u_{[i]}` of the observation
:math:`u_i` is defined as the position of :math:`u_i` in the sample
reordered in ascending order: if :math:`u_i` is the smallest value in
the sample :math:`(u_1,\ldots,u_N)`, its ranking would equal 1; if
:math:`u_i` is the second smallest value in the sample, its ranking
would equal 2, and so forth. The ranking transformation is a procedure
that takes the sample :math:`(u_1,\ldots,u_N)`) as input data and
produces the sample :math:`(u_{[1]},\ldots,u_{[N]})` as an output
result.

For example, let us consider the sample
:math:`(u_1,u_2,u_3,u_4) = (1.5,0.7,5.1,4.3)`. We therefore have
:math:`(u_{[1]},u_{[2]}u_{[3]},u_{[4]}) = (2,1,4,3)`. :math:`u_1 = 1.5`
is in fact the second smallest value in the original, :math:`u_2 = 0.7`
the smallest, etc.

The estimation of Spearman’s correlation coefficient is therefore equal
to Pearson’s coefficient estimated with the aid of the :math:`N` pairs
:math:`(u_{[1]},v_{[1]})`, :math:`(u_{[2]},v_{[2]})`, …,
:math:`(u_{[N]},v_{[N]})`:

.. math::

   \begin{aligned}
       \widehat{\rho}^S_{U,V} = \frac{ \displaystyle \sum_{i=1}^N \left( u_{[i]} - \overline{u}_{[]} \right) \left( v_{[i]} - \overline{v}_{[]} \right) }{ \sqrt{\displaystyle \sum_{i=1}^N \left( u_{[i]} - \overline{u}_{[]} \right)^2 \left( v_{[i]} - \overline{v}_{[]} \right)^2} }
     \end{aligned}

where :math:`\overline{u}_{[]}` and :math:`\overline{v}_{[]}` represent
the empirical means of the samples :math:`(u_{[1]},\ldots,u_{[N]})` and
:math:`(v_{[1]},\ldots,v_{[N]})`.

The Spearman’s correlation coefficient takes values between -1 and 1.
The closer its absolute value is to 1, the stronger the indication is
that a monotonic relationship exists between variables :math:`U` and
:math:`V`. The sign of Spearman’s coefficient indicates if the two
variables increase or decrease in the same direction (positive
coefficient) or in opposite directions (negative coefficient). We note
that a correlation coefficient equal to 0 does not necessarily imply the
independence of variables :math:`U` and :math:`V`. There are two
possible situations in the event of a zero Spearman’s correlation
coefficient:

-  the variables :math:`U` and :math:`V` are in fact independent,

-  or a non-monotonic relationship exists between :math:`U` and
   :math:`V`.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    N = 20
    ot.RandomGenerator.SetSeed(10)
    x = ot.Uniform(0.0, 10.0).getSample(N)
    f = ot.SymbolicFunction(['x'], ['x^2'])
    y = f(x) + ot.Normal(0.0, 5.0).getSample(N)
    graph = f.draw(0.0, 10.0)
    graph.setTitle('There is a monotonic relationship between U and V:\nSpearman\'s coefficient is a relevant measure of dependency...')
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
    f = ot.SymbolicFunction(['x'], ['5*x+10'])
    y = f(x) + ot.Normal(0.0, 5.0).getSample(N)
    graph = f.draw(0.0, 10.0)
    graph.setTitle('... because the rank transformation turns any monotonic trend\ninto a linear relation for which Pearson\'s correlation is relevant')
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
    graph.setTitle('nSpearman\'s coefficient estimate is close to zero\nbecause U and V are independent')
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
    graph.setTitle('Spearman\'s coefficient estimate is quite close to zero\neven though U and V are not independent')
    graph.setXTitle('u')
    graph.setYTitle('v')
    cloud = ot.Cloud(x, y)
    cloud.setPointStyle('circle')
    cloud.setColor('orange')
    graph.add(cloud)
    View(graph)

Spearman’s coefficient is often referred to as the rank correlation
coefficient.


.. topic:: API:

    - See :class:`~openturns.CorrelationAnalysis_SpearmanCorrelation`

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/estimate_spearman`

.. topic:: References:

    - [Saporta1990]_
    - [Dixon1983]_
    - [NISThandbook]_
    - [Dagostino1986]_
    - [Bhattacharyya1997]_
    - [Sprent2001]_
    - [Burnham2002]_
