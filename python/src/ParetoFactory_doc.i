%feature("docstring") OT::ParetoFactory
R"RAW(Pareto factory.


Notes
-----
Several estimators to build a Pareto distribution from a scalar sample
are proposed. The default strategy is to use the least squares estimator.

**Moments based estimator:**

Lets denote:

- :math:`\displaystyle \overline{x}_{\sampleSize} = \frac{1}{\sampleSize} \sum_{i=1}^\sampleSize x_i` the empirical mean of the sample, 
- :math:`\displaystyle s_\sampleSize^2 = \frac{1}{\sampleSize - 1} \sum_{i=1}^\sampleSize (x_i - \overline{x}_\sampleSize)^2` its empirical variance,
- :math:`\displaystyle \widehat{\text{skew}}_{\sampleSize}` the empirical skewness of the sample

The estimator :math:`(\hat{\beta}_{\sampleSize}, \hat{\alpha}_{\sampleSize}, \hat{\gamma}_{\sampleSize})` of
:math:`(\beta, \alpha, \gamma)` is defined as follows :

The parameter :math:`\hat{\alpha}_{\sampleSize}` is solution of the equation: 

.. math::
    :nowrap:

    \begin{eqnarray*}
        \widehat{\text{skew}}_n & =  & \dfrac{ 2(1+\hat{\alpha}_{\sampleSize}) }{ \hat{\alpha}_n - 3 } \sqrt{ \dfrac{ \hat{\alpha}_n - 2 }{ \hat{\alpha}_n } } 
    \end{eqnarray*}

There exists a symbolic solution. If :math:`\hat{\alpha}_n > 3`, then we get :math:`(\hat{\beta}_n, \hat{\gamma}_n)` as follows: 

.. math::
    :nowrap:

    \begin{eqnarray*}
       \hat{\beta}_n & = & (\hat{\alpha}_n-1) \sqrt{\dfrac{\hat{\alpha}_n-2}{\hat{\alpha}_n}}s_n \\
       \hat{\gamma}_n & = & \overline{x}_n - \dfrac{\hat{\alpha}_n}{\hat{\alpha}_n+1} \hat{\beta}_n
    \end{eqnarray*}


**Maximum likelihood based estimator:**

The likelihood of the sample is defined by:

.. math::

    \ell(\alpha, \beta, \gamma|  x_1, \dots, x_n) = n\log \alpha + n\alpha \log \beta - (\alpha+1) \sum_{i=1}^n \log(x_i-\gamma)


The maximum likelihood based estimator :math:`(\hat{\beta}_n, \hat{\alpha}_n, \hat{\gamma}_n)` of :math:`(\beta, \alpha, \gamma)` maximizes the likelihood:

.. math::

    (\hat{\beta}_n, \hat{\alpha}_n, \hat{\gamma}_n) = \argmax_{\alpha, \beta, \gamma} \ell(\alpha, \beta, \gamma|  x_1, \dots, x_n)

The following strategy is to be implemented soon: 
For a given :math:`\gamma`, the likelihood of the sample is defined by:

.. math::

    \ell(\alpha(\gamma), \beta(\gamma)|  x_1, \dots, x_n, \gamma) = n\log \alpha(\gamma) + n\alpha(\gamma) \log \beta(\gamma) - (\alpha(\gamma)+1) \sum_{i=1}^n \log(x_i-\gamma)

We get :math:`(\hat{\beta}_n( \gamma), \hat{\alpha}_n( \gamma))` which maximizes :math:`\ell(\alpha, \beta|  x_1, \dots, x_n, \gamma)` :

.. math::

    (\hat{\beta}_n( \gamma), \hat{\alpha}_n( \gamma)) = \argmax_{\alpha, \beta}   \ell(\alpha(\gamma), \beta(\gamma)|  x_1, \dots, x_n, \gamma) \text{ under the constraint } \gamma + \hat{\beta}_n(\gamma) \leq x_{(1,n)}

We get:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \hat{\beta}_n( \gamma) & = & x_{(1,n)} - \gamma \\
         \hat{\alpha}_n( \gamma) & = & \dfrac{n}{\sum_{i=1}^n \log\left( \dfrac{x_i - \gamma}{\hat{\beta}_n( \gamma)}\right)}
    \end{eqnarray*}


Then the parameter :math:`\gamma` is obtained by maximizing the likelihood :math:`\ell(\hat{\beta}_n( \gamma), \hat{\alpha}_n( \gamma), \gamma)`:

.. math::

    \hat{\gamma}_n = \argmax_{\gamma}  \ell(\hat{\beta}_n( \gamma), \hat{\alpha}_n( \gamma), \gamma)

The initial point of the optimisation problem is :math:`\gamma_0 = x_{(1,n)} - |x_{(1,n)}|/(2+n)`.


**Least squares estimator:**

The parameter :math:`\gamma` is numerically optimized by non-linear least-squares:

.. math::

    \min{\gamma} \norm{\hat{S}_n(x_i) - (a_1 \log(x_i - \gamma) + a_0)}_2^2


where :math:`a_0, a_1` are computed from linear least-squares at each optimization evaluation.

When :math:`\gamma` is known and the :math:`x_i` follow a Pareto distribution then
we use linear least-squares to solve the relation:

.. math::
  :label: least_squares_estimator_pareto

   \hat{S}_n(x_i) = a_1 \log(x_i - \gamma) + a_0

And the remaining parameters are estimated with:

.. math::

    \hat{\beta} &= \exp{\frac{-a_0}{a_1}}\\
    \hat{\alpha} &= -a_1


See also
--------
DistributionFactory, Normal

Examples
--------

In the first example, we estimate all the parameters.

>>> import openturns as ot
>>> real_distribution = ot.Pareto(2.5, 1.0, 0.0)
>>> sample = real_distribution.getSample(1000)
>>> factory = ot.ParetoFactory()
>>> estimated_distribution_full = factory.build(sample)

In this example, we assume that the gamma parameter is known
and estimate alpha and beta.
The user sets the value of gamma (index 2 in the order alpha, beta, gamma).

>>> known_gamma = 0.0
>>> factory.setKnownParameter([known_gamma], [2])
>>> estimated_distribution_fixed = factory.build(sample)

)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::ParetoFactory::buildMethodOfLikelihoodMaximization
"Method of likelihood maximization.

Refer to :class:`~openturns.MaximumLikelihoodFactory`.

Parameters
----------
sample : 2-d sequence of float
    Data.

Returns
-------
distribution : :class:`~openturns.Pareto`
    The estimated distribution"

// ----------------------------------------------------------------------------

%feature("docstring") OT::ParetoFactory::buildMethodOfMoments
"Method of moments estimator.


Parameters
----------
sample : 2-d sequence of float
    Data.

Returns
-------
distribution : :class:`~openturns.Pareto`
    The estimated distribution"

// ----------------------------------------------------------------------------

%feature("docstring") OT::ParetoFactory::buildMethodOfLeastSquares
"Method of least-squares.

Notes
-----
Refer to :class:`~openturns.LeastSquaresDistributionFactory`.

Parameters
----------
sample : 2-d sequence of float
    Data.
gamma : float, optional
    Gamma parameter.

Returns
-------
distribution : :class:`~openturns.Pareto`
    The estimated distribution.

Examples
--------

In the following example, the parameters of a :class:`~openturns.Pareto` 
are estimated from a sample. 
We create a simulated sample from a Pareto distribution with
arameters alpha=2.5, beta=1.0 and gamma=0.0.

>>> import openturns as ot
>>> real_distribution = ot.Pareto(2.5, 1.0, 0.0)
>>> sample = real_distribution.getSample(1000)
>>> factory = ot.ParetoFactory()

Example 1: When gamma is known.
In this case, we estimate of parameters alpha and beta using 
linear least squares.

>>> estimated_distribution_fixed = factory.buildMethodOfLeastSquares(sample, known_gamma)
>>> print(estimated_distribution_fixed.getParameter())
[2.46372,0.979926,0]

Example 2: When gamma is unknown.
In this case, we perform a full estimation by non-linear least squares (for gamma)
combined with linear least squares (for alpha and beta).

>>> estimated_distribution_full = factory.buildMethodOfLeastSquares(sample)
>>> print(estimated_distribution_full.getParameter())
[2.26187,0.951929,0.268134]
"

// ----------------------------------------------------------------------------

%feature("docstring") OT::ParetoFactory::buildAsPareto
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsPareto()

    buildAsPareto(*sample*)

    buildAsPareto(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float,
    The parameters of the :class:`~openturns.Pareto`.

Returns
-------
dist : :class:`~openturns.Pareto`
    The estimated distribution as a Pareto.
    
    In the first usage, the default Pareto distribution is built."

