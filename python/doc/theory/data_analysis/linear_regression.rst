.. _linear_regression:

Linear regression
-----------------

This method deals with the parametric modelling of a probability
distribution for a random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`. It aims to measure
a type of dependence (here a linear relation) which may exist between a
component :math:`X^i` and other uncertain variables :math:`X^j`.

The principle of the multiple linear regression model is to find the
function that links the variable :math:`X^i` to other variables
:math:`X^{j_1}`,…,\ :math:`X^{j_K}` by means of a linear model:

.. math::

    \begin{aligned}
       X^i = a_0 + \sum_{j \in \{ j_1,\ldots,j_K \} } a_j X^j + \varepsilon
     \end{aligned}

where :math:`\varepsilon` describes a random variable with zero mean
and standard deviation :math:`\sigma` independent of the input variables
:math:`X^i`. For given values of :math:`X^{j_1}`,…,\ :math:`X^{j_K}`,
the average forecast of :math:`X^i` is denoted by :math:`\widehat{X}^i`
and is defined as:

.. math::

    \begin{aligned}
       \widehat{X}^i = a_0 + \sum_{j \in \{ j_1,\ldots,j_K \} } a_j X^j
     \end{aligned}

The estimators for the regression coefficients
:math:`\widehat{a}_0,\widehat{a}_1,\ldots,\widehat{a}_{K}`, and the
standard deviation :math:`\sigma` are obtained from a sample of
:math:`(X^i,X^{j_1},\ldots,X^{j_K})`, that is a set of :math:`N` values
:math:`(x^i_1,x_1^{j_1},\ldots,x_1^{j_K})`,…,\ :math:`(x^i_n,x_n^{j_1},\ldots,x_n^{j_K})`.
They are determined via the least-squares method:

.. math::

    \begin{aligned}
       \left\{ \widehat{a}_0,\widehat{a}_1,\ldots,\widehat{a}_{K} \right\} = \textrm{argmin} \sum_{k=1}^n \left[ x^i_k - a_0 - \sum_{j \in \{ j_1,\ldots,j_K \} } a_j x^j_k \right]^2
     \end{aligned}

In other words, the principle is to minimize the total quadratic
distance between the observations :math:`x^i_k` and the linear forecast
:math:`\widehat{x}^i_k`.

Some estimated coefficient :math:`\widehat{a}_\ell` may be close to
zero, which may indicate that the variable :math:`X^{j_\ell}` does not
bring valuable information to forecast :math:`X^i`. A classical statistical
test to identify such situations is available: Fisher’s test.
For each estimated coefficient :math:`\widehat{a}_\ell`, an important
characteristic is the so-called “:math:`p`-value” of Fisher’s test. The
coefficient is said to be “significant” if and only if
:math:`\alpha_{\ell \textrm{lim}}` is greater than a value
:math:`\alpha` chosen by the user (typically 5% or 10%). The higher the
:math:`p`-value, the more significant the coefficient.

Another important characteristic of the adjusted linear model is the
coefficient of determination :math:`R^2`. This quantity indicates the
part of the variance of :math:`X^i` that is explained by the linear
model:

.. math::

    \begin{aligned}
       R^2 = \frac{ \displaystyle \sum_{k=1}^n \left( x^i_k - \overline{x}^i \right)^2 - \sum_{k=1}^n \left( x^i_k - \widehat{x}_k^i \right)^2 }{ \sum_{k=1}^n \left( x^i_k - \overline{x}^i \right)^2 }
     \end{aligned}

where :math:`\overline{x}^i` denotes the empirical mean of the sample
:math:`\left\{ x^i_1,\ldots,x^i_n  \right\}`.

Thus, :math:`0 \leq R^2 \leq 1`. A value close to 1 indicates a good fit
of the linear model, whereas a value close to 0 indicates that the
linear model does not provide a relevant forecast. A statistical test
allows to detect significant values of :math:`R^2`. Again, a
:math:`p`-value is provided: the higher the :math:`p`-value, the more
significant the coefficient of determination.

By definition, the multiple regression model is only relevant for linear
relationships, as in the following simple example where
:math:`X^2 = a_0 + a_1 X^1`.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    N = 1000
    #create a sample X
    dist = ot.Triangular(1.0, 5.0, 10.0)
    # create a Y sample : Y = 0.5 + 3 * X + eps
    eps = ot.Normal(0.0, 1.0)
    sample = ot.ComposedDistribution([dist, eps]).getSample(N)
    f = ot.SymbolicFunction(['x', 'eps'], ['0.5+3.0*x+eps'])
    sampleY = f(sample)
    sampleX = sample.getMarginal(0)
    sampleX.setName('X')
    # Fit this linear model
    regressionModel = ot.LinearModelAlgorithm(sampleX, sampleY).getResult()
    # Test the linear model fitting
    graph = ot.VisualTest.DrawLinearModel(sampleX, sampleY, regressionModel)
    cloud = graph.getDrawable(0)
    cloud.setPointStyle('times')
    graph.setDrawable(cloud, 0)
    graph.setTitle('')
    View(graph)


In this second example (still in dimension 1), the linear model is not
relevant because of the exponential shape of the relation. But a linear
approach would be useful on the transformed problem
:math:`X^2 = a_0 + a_1 \exp X^1`. In other words, what is important is
that the relationships between :math:`X^i` and the variables
:math:`X^{j_1}`,…,\ :math:`X^{j_K}` is linear with respect to the
regression coefficients :math:`a_j`.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    N = 1000
    #create a sample X
    dist = ot.Triangular(1.0, 5.0, 10.0)
    # create a Y sample : Y = exp(X/2) + eps
    eps = ot.Normal(0.0, 1.0)
    sample = ot.ComposedDistribution([dist, eps]).getSample(N)
    f = ot.SymbolicFunction(['x', 'eps'], ['exp(0.5*x)+eps'])
    sampleY = f(sample)
    sampleX = sample.getMarginal(0)
    sampleX.setName('X')
    # same as good test
    regressionModel = ot.LinearModelAlgorithm(sampleX, sampleY).getResult()
    graph = ot.VisualTest.DrawLinearModel(sampleX, sampleY, regressionModel)
    cloud = graph.getDrawable(0)
    cloud.setPointStyle('times')
    graph.setDrawable(cloud, 0)
    graph.setTitle('')
    View(graph)


The value of :math:`R^2` is a good indication of the goodness-of fit of
the linear model. However, several other verifications have to be
carried out before concluding that the linear model is satisfactory. For
instance, one has to pay attentions to the “residuals”
:math:`\{ u_1,\ldots,u_N \}` of the regression:

.. math::

    \begin{aligned}
       u_j = x^i - \widehat{x}^i
     \end{aligned}

A residual is thus equal to the difference between the observed value
of :math:`X^i` and the average forecast provided by the linear model. A
key-assumption for the robustness of the model is that the
characteristics of the residuals do not depend on the value of
:math:`X^i,X^{j_1},\dots,X^{j_K}`: the mean value should be close
to 0 and the standard deviation should be constant. Thus, plotting the
residuals versus these variables can fruitful.

In the following example, the behaviour of the residuals is
satisfactory: no particular trend can be detected neither in the mean
nor in he standard deviation.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    N = 1000
    #create a sample X
    dist = ot.Triangular(1.0, 5.0, 10.0)
    # create a Y sample : Y = 0.5 + 3 * X + eps
    eps = ot.Normal(0.0, 1.0)
    sample = ot.ComposedDistribution([dist, eps]).getSample(N)
    f = ot.SymbolicFunction(['x', 'eps'], ['0.5+3.0*x+eps'])
    sampleY = f(sample)
    sampleX = sample.getMarginal(0)
    sampleX.setName('X')
    #create a linear model
    regressionModel = ot.LinearModelAlgorithm(sampleX, sampleY).getResult()
    graph = ot.VisualTest.DrawLinearModelResidual(sampleX, sampleY, regressionModel)
    cloud = graph.getDrawable(0)
    cloud.setPointStyle('times')
    graph.setDrawable(cloud, 0)
    graph.setTitle('')
    # copy the graph in a file
    View(graph)


The next example illustrates a less favourable situation: the mean value
of the residuals seems to be close to 0 but the standard deviation tends
to increase with :math:`X`. In such a situation, the linear model should
be abandoned, or at least used very cautiously.

.. plot::

    import openturns as ot
    from openturns.viewer import View

    N = 1000
    #create a sample X
    dist = ot.Triangular(1.0, 5.0, 10.0)
    # create a Y sample : Y = exp(X/2) + eps
    eps = ot.Normal(0.0, 1.0)
    sample = ot.ComposedDistribution([dist, eps]).getSample(N)
    f = ot.SymbolicFunction(['x', 'eps'], ['exp(0.5*x)+eps'])
    sampleY = f(sample)
    sampleX = sample.getMarginal(0)
    sampleX.setName('X')
    # same as good test
    regressionModel = ot.LinearModelAlgorithm(sampleX, sampleY).getResult()
    graph = ot.VisualTest.DrawLinearModelResidual(sampleX, sampleY, regressionModel)
    cloud = graph.getDrawable(0)
    cloud.setPointStyle('times')
    graph.setDrawable(cloud, 0)
    graph.setTitle('')
    View(graph)


.. topic:: API:

    - See :class:`~openturns.LinearModelAlgorithm` to build a linear model
    - See :class:`~openturns.LinearModelResult` for the associated results
    - See :py:func:`~openturns.VisualTest_DrawLinearModel` to draw a linear model
    - See :py:func:`~openturns.VisualTest_DrawLinearModelResidual` to draw the residual
    - See :py:func:`~openturns.LinearModelTest_LinearModelFisher` to assess the nullity of the coefficients
    - See :py:func:`~openturns.LinearModelTest_LinearModelResidualMean` to assess the mean residual
    - See :py:func:`~openturns.LinearModelTest_LinearModelHarrisonMcCabe` to assess the homoskedasticity of the residual
    - See :py:func:`~openturns.LinearModelTest_LinearModelBreuschPagan` to assess the homoskedasticity of the residual
    - See :py:func:`~openturns.LinearModelTest_LinearModelDurbinWatson` to assess the autocorrelation of the residual

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/linear_regression`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
    - [nisthandbook]_
    - [dagostino1986]_
    - [bhattacharyya1997]_
    - [sprent2001]_
    - [burnham2002]_
