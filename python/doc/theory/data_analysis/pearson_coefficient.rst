.. _pearson_coefficient:

Pearson correlation coefficient
-------------------------------

The Pearson correlation coefficient :math:`\rho^P(X,Y)`  measures
the strength of a linear relationship between two random variables
:math:`X` and :math:`Y` with finite variance. It is defined as follows:

.. math::
       \rho^P(X,Y)= \dfrac{\Cov{X,Y}}{\sqrt{\Var{X}\Var{Y}}}

where
:math:`\Cov{X,Y} = \Expect{ \left( X - \Expect{X} \right) \left( Y - \Expect{Y} \right) }`.

Let :math:`((x_1, y_1), \dots, (x_\sampleSize, y_\sampleSize))` be a sample generated from
:math:`(X,Y)`. The Pearson  correlation coefficient is estimated:

.. math::
    :label: PearsonEstim
    \hat{\rho}^P(X,Y) = \dfrac{\sum_{k=1}^\sampleSize (x_k- \bar{X})(y_k- \bar{Y})}
    {\sqrt{\sum_{k=1}^\sampleSize(x_k- \bar{X})^2\sum_{k=1}^\sampleSize(y_k- \bar{Y})^2}}

where :math:`\bar{X} = \dfrac{1}{\sampleSize} \sum_{k=1}^\sampleSize x_k` and
:math:`\bar{Y} = \dfrac{1}{\sampleSize} \sum_{k=1}^\sampleSize y_k` are the mean of each sample.

We sum up some interesting features of the coefficient:

- The Pearson’s correlation coefficient takes values between -1 and 1.

- If :math:`|\rho_P(X,Y)|=1` then there exists a linear relationship
  between :math:`X` and :math:`Y`.

- The closer its absolute value is to 1, the stronger the indication is
  that a linear relationship exists between variables :math:`X` and
  :math:`Y`. The sign of the Pearson’s coefficient indicates if the two
  variables increase or decrease in the same direction (positive
  coefficient) or in opposite directions (negative coefficient).

- If :math:`X` and :mat:`Y` are independent, then :math:`\rho_P(X,Y)|=0`.

- If :math:`\rho_P(X,Y)|=0`, it does not imply the independence of the variables
  :math:`X` and :math:`Y`. It may only means that the relation between both variables
  is not linear.

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

    - See method :py:meth:`~openturns.CorrelationAnalysis.computeLinearCorrelation`
    - See method :py:meth:`~openturns.Sample.computeLinearCorrelation`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/sample_analysis/plot_sample_correlation`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
    - [nisthandbook]_
    - [dagostino1986]_
    - [bhattacharyya1997]_
    - [sprent2001]_
    - [burnham2002]_
