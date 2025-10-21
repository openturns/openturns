.. _spearman_coefficient:

Spearman correlation coefficient
--------------------------------

The Spearman rank correlation coefficient measures how strongly
two random variables with finite variance are correlated. Spearman's correlation assesses monotonic
relationships between both variables.

Let :math:`(X,Y)` be two random variables which CDF are denoted by :math:`F_X` and :math:`F_Y`.
Spearman’s rank correlation coefficient :math:`\rho^S(X,Y)` is defined by:

.. math::
    \rho^S(X,Y) = \dfrac{\Cov{F_X(X),F_Y(Y)}}{\sqrt{\Var{F_X(X)}\Var{F_Y(Y)}}}

where :math:`\Cov{.}` is the covariance operator and
:math:`F_X` and :math:`F_Y` are the  respective CDF of :math:`X` and :math:`Y`.

The Spearman correlation between two variables is equal to the
:ref:`Pearson correlation coefficient <pearson_coefficient>` between the rank values of the variables:

.. math::
    \rho^S(X,Y) = \rho^P(F_X(X), F_Y(Y))


If :math:`C` is the CDF of the copula of the random vector :math:`(X,Y)`, then we get:

.. math::
   \rho^S(X,Y) = \rho(F_X(X),F_Y(Y)) = 12 \iint_{[0,1]^2} C(u,v)\,du\,dv - 3

which shows that the  Spearman correlation is linked to the copula only.

Let :math:`((x_1, y_1), \dots, (x_\sampleSize, y_\sampleSize))` be a sample generated from
:math:`(X,Y)`. We denote by :math:`(r_1, s_1), \dots, (r_\sampleSize, s_\sampleSize)` the rank sample,
which means that :math:`r_k` is the rank of the value :math:`x_k` within the sample
:math:`(x_1, \dots, x_\sampleSize)` and :math:`s_k` is the rank of the value :math:`y_k` within the
sample :math:`(y_1, \dots, y_\sampleSize)`. The estimator :math:`\hat{\rho}^S(X,Y)` is equal to the
estimator  :math:`\hat{\rho}^P(X,Y)` computed
on the rank sample :math:`(r_1, s_1), \dots, (r_\sampleSize, s_\sampleSize)`. It is estimated as follows:

.. math::
    :label: SpearmanEstim
    \hat{\rho}^S(X,Y) = \dfrac{\sum_{k=1}^\sampleSize (r_k- \bar{r})(s_k- \bar{s})}
    {\sqrt{\sum_{k=1}^\sampleSize(r_k- \bar{r})^2\sum_{k=1}^\sampleSize(s_k- \bar{s})^2}}

where :math:`\bar{r} = \dfrac{1}{\sampleSize} \sum_{k=1}^\sampleSize r_k` and
:math:`\bar{s} = \dfrac{1}{\sampleSize} \sum_{k=1}^\sampleSize s_k` are the mean rank of each sample.


We sum up some interesting features of the coefficient:

- The Spearman correlation coefficient takes values between -1 and 1.

- If :math:`|\rho_S(X,Y)|=1` then there exists a monotonic function
  :math:`\varphi` such that :math:`Y=\varphi(X)`.

- The closer its absolute value is to 1, the stronger the indication is
  that a monotonic relationship exists between variables :math:`X` and
  :math:`Y`. The sign of the Spearman coefficient indicates if the two
  variables increase or decrease in the same direction (positive
  coefficient) or in opposite directions (negative coefficient).

- If :math:`X` and :mat:`Y` are independent, then :math:`\rho_S(X,Y)|=0`.

- If :math:`\rho_S(X,Y)|=0`, it does not imply the independence of the variables
  :math:`X` and :math:`Y`. It may only means that the relation between both variables
  is not monotonic.

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

    - See method :py:meth:`~openturns.CorrelationAnalysis.computeSpearmanCorrelation`
    - See method :py:meth:`~openturns.Sample.computeSpearmanCorrelation`

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
