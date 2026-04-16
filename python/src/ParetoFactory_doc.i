%feature("docstring") OT::ParetoFactory
R"RAW(Pareto factory.


Notes
-----
Several estimators to build a Pareto distribution from a scalar sample
are proposed. The default strategy is to use the least squares estimator.
We make the assumption that :math:`(x_1, \dots, x_{\sampleSize})` is an i.i.d. sample
from the Pareto random variable where :math:`\sampleSize` is the sample size.

**Moments based estimator:**

Let us define the sample statistics required for the estimation.
The empirical mean :math:`\overline{x}` is calculated as:

.. math::

    \overline{x} = \frac{1}{\sampleSize} \sum_{i=1}^\sampleSize x_i

The associated empirical standard deviation :math:`s` is:

.. math::

    s = \sqrt{\frac{1}{\sampleSize - 1} \sum_{i=1}^\sampleSize (x_i - \overline{x})^2}.

Finally, the distribution of the sample is characterized by its empirical
skewness, denoted as :math:`\widehat{\text{skew}}`.

The estimator :math:`\left(\widehat{\beta}, \widehat{\alpha}, \widehat{\gamma}\right)` of
:math:`(\beta, \alpha, \gamma)` is defined as follows.
The parameter :math:`\widehat{\alpha}` is solution of the equation:

.. math::

    \widehat{\text{skew}} 
    =  \dfrac{ 2(1 + \widehat{\alpha}) }{ \widehat{\alpha} - 3 } \sqrt{ \dfrac{ \widehat{\alpha} - 2 }{ \widehat{\alpha} } }.

The previous nonlinear equation is solved using a numerical method.
If :math:`\widehat{\alpha} \leq 3`, then an exception is raised.
If :math:`\widehat{\alpha} > 3`, then we compute
:math:`(\widehat{\beta}, \widehat{\gamma})` as follows:

.. math::

    \widehat{\beta}
    & = s (\widehat{\alpha} - 1) \sqrt{\dfrac{\widehat{\alpha} - 2}{\widehat{\alpha}}}, \\
    \widehat{\gamma}
    & = \overline{x} - \dfrac{\widehat{\alpha} \widehat{\beta}}{\widehat{\alpha}+1}.

**Least squares estimator:**

Before introducing the equations, let us present the overall
methodology.
When :math:`\gamma` is known, then we solve a linear least squares problem
to estimate :math:`\beta` and :math:`\alpha`.

When :math:`\gamma` is unknown, then two problems are involved:

- in the outer loop, a non-linear least squares problem is solved
  to estimate :math:`\gamma`;
- in the inner loop, for a given value of :math:`\gamma`,
  a linear least squares problem is solved
  to estimate :math:`\alpha` and :math:`\beta`.

Let us now introduce the methods in more details.
Let :math:`\widehat{S}` be the empirical survival function.
If :math:`\gamma` is known, then we solve the following linear least-squares problem:

.. math::
  :label: least_squares_estimator_pareto

    \left(\widehat{a}_0, \widehat{a}_1\right)
    = \argmin_{(a_0, a_1)^\top \in \Rset^2} \sum_{i = 1}^\sampleSize
        \left(\widehat{S}(x_i) - (a_1 \log(x_i - \gamma) + a_0)\right)^2.

To do this, let :math:`\vect{y} \in \Rset^{\sampleSize}` be the vector
equal to the value of the empirical survival function at each observation:

.. math::

    y_i = \widehat{S}(x_i)

for :math:`1 \leq i \leq \sampleSize`.
Moreover, let :math:`\vect{z} \in \Rset^{\sampleSize}` be the 
vector of logarithm of the shifted observations:

.. math::

    z_i = \log(x_i - \gamma)

for :math:`1 \leq i \leq \sampleSize`.
Then the linear least squares problem is:

.. math::

    \left(\widehat{a}_0, \widehat{a}_1\right)
    = \argmin_{(a_0, a_1)^\top \in \Rset^2} \sum_{i = 1}^\sampleSize
        \left(y_i - (a_1 z_i + a_0)\right)^2.

See :class:`~openturns.LinearLeastSquares` for more details.

Once the vector :math:`\left(\widehat{a}_0, \widehat{a}_1\right)` is computed,
we compute :math:`\alpha` and :math:`\beta` from the equations:

.. math::

    \widehat{\beta} &= \exp \left( \frac{-\widehat{a}_0}{\widehat{a}_1} \right), \\
    \widehat{\alpha} &= -\widehat{a}_1.

When :math:`\gamma` is unknown, it is estimated using non-linear
least squares.
More precisely, the parameter :math:`\gamma` is the solution of:

.. math::

    \widehat{\gamma} 
    = \argmin_{\gamma} \sum_{i = 1}^\sampleSize
      \left(\widehat{S}(x_i) - (a_1(\gamma) \log(x_i - \gamma) + a_0(\gamma))\right)^2.


where :math:`a_0, a_1` are computed from linear least-squares at each optimization evaluation.

**Maximum likelihood based estimator:**

The log-likelihood of the sample is:

.. math::

    & \ell(\beta, \alpha, \gamma \mid  x_1, \dots, x_{\sampleSize}) \\
    & = \sampleSize \log(\alpha) + \sampleSize \alpha \log(\beta)
      - (\alpha + 1) \sum_{i = 1}^\sampleSize \log(x_i - \gamma)


The maximum likelihood based estimator
:math:`\left(\widehat{\beta}, \widehat{\alpha}, \widehat{\gamma}\right)`
of :math:`\left(\beta, \alpha, \gamma\right)` maximizes the log-likelihood:

.. math::

    \left(\widehat{\beta}, \widehat{\alpha}, \widehat{\gamma}\right)
    = \argmax_{\beta, \alpha, \gamma} \ell(\beta, \alpha, \gamma \mid  x_1, \dots, x_{\sampleSize})

In the current implementation, all parameters are estimated simultaneously.

However, another method could be used, which can be described as follows.
For a given value of :math:`\gamma`, the log-likelihood of the sample is
defined by:

.. math::

    & \ell(\alpha(\gamma), \beta(\gamma) \mid  x_1, \dots, x_{\sampleSize}, \gamma) \\
    & = \sampleSize \log(\alpha(\gamma)) + \sampleSize \alpha(\gamma) \log(\beta(\gamma))
      - (\alpha(\gamma) + 1) \sum_{i=1}^\sampleSize \log(x_i - \gamma)

We compute :math:`\left(\widehat{\beta}( \gamma), \widehat{\alpha}( \gamma)\right)` which
maximizes :math:`\ell(\beta, \alpha, \mid  x_1, \dots, x_{\sampleSize}, \gamma)`:

.. math::

    \begin{aligned}
    \left(\widehat{\beta}(\gamma), \widehat{\alpha}(\gamma)\right)
    & = \argmax_{(\beta, \alpha)^\top \in \Rset^2} & & \ell(\alpha(\gamma), \beta(\gamma) \mid x_1, \dots, x_{\sampleSize}, \gamma) \\
	& \quad \text{s.t.} & & \gamma + \widehat{\beta}(\gamma) \leq x_{(1,\sampleSize)}
    \end{aligned}

where :math:`x_{(1,\sampleSize)}` is the smallest observation in the sample:

.. math::

    x_{(1,\sampleSize)} = \min_{1 \leq i \leq \sampleSize} x_i.

We get:

.. math::

    \widehat{\beta}( \gamma) & = x_{(1,\sampleSize)} - \gamma, \\
    \widehat{\alpha}( \gamma) & = \dfrac{\sampleSize}{\sum_{i=1}^\sampleSize
        \log\left( \dfrac{x_i - \gamma}{\widehat{\beta}( \gamma)}\right)}.

Then the parameter :math:`\gamma` is computed by maximizing the log-likelihood
of the sample :math:`\ell\left(\widehat{\beta}( \gamma), \widehat{\alpha}( \gamma), \gamma\right)`:

.. math::

    \widehat{\gamma}
    = \argmax_{\gamma \in \Rset} \ell\left(\widehat{\beta}( \gamma), \widehat{\alpha}( \gamma), \gamma\right)

The starting point of the optimization algorithm is:

.. math::

    \gamma_0 = x_{(1,\sampleSize)} - \frac{|x_{(1,\sampleSize)}|}{2 + \sampleSize}.

See also
--------
Pareto, DistributionFactory

Examples
--------

In the first example, we estimate all the parameters, that is,
:math:`\beta`, :math:`\alpha` and :math:`\gamma`.

>>> import openturns as ot
>>> real_distribution = ot.Pareto(2.5, 1.0, 0.0)
>>> sample = real_distribution.getSample(1000)
>>> factory = ot.ParetoFactory()
>>> estimated_distribution_full = factory.build(sample)

In the second example, we assume that the :math:`\gamma` parameter is known
and estimate :math:`\beta` and :math:`\alpha`.
The user sets the value of :math:`\gamma` (index 2 in the order :math:`\beta`, 
:math:`\alpha` and :math:`\gamma`).

>>> known_gamma = 0.0
>>> factory.setKnownParameter([2], [known_gamma])
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
    The estimated distribution

Note
----
When this method is used, all parameters are estimated
simultaneously.
"

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
R"RAW(Method of least-squares.

Parameters
----------
sample : 2-d sequence of float
    Data.
gamma : float, optional
    Location parameter.
    If provided, the estimation of :math:`\beta` and :math:`\alpha` is performed
    via linear least squares with :math:`\gamma` fixed.
    If not specified, :math:`\gamma` is first estimated using a
    non-linear least squares routine, followed by a linear least
    squares estimation for the remaining parameters.

Returns
-------
distribution : :class:`~openturns.Pareto`
    The estimated distribution.

Examples
--------

In the following example, the parameters of a :class:`~openturns.Pareto` 
are estimated from a sample. 
We create a simulated sample from a Pareto distribution with
parameters `beta=2.5`, `alpha=1.0` and `gamma=0.0`.

>>> import openturns as ot
>>> real_distribution = ot.Pareto(2.5, 1.0, 0.0)
>>> sample = real_distribution.getSample(1000)
>>> factory = ot.ParetoFactory()

**Example 1:** When gamma is known.
In this case, we estimate the parameters beta and alpha using 
linear least squares.

>>> known_gamma = 0.0
>>> estimated_distribution_fixed = factory.buildMethodOfLeastSquares(sample, known_gamma)
>>> print(estimated_distribution_fixed.getParameter())
[2.53...,1.03...,0]

**Example 2:** When gamma is unknown.
In this case, we perform a full estimation by non-linear least squares (for gamma)
combined with linear least squares (for beta and alpha).

>>> estimated_distribution_full = factory.buildMethodOfLeastSquares(sample)
>>> print(estimated_distribution_full.getParameter())
[2.61...,1.05...,-0.10...]
)RAW"

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

