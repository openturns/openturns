%feature("docstring") OT::EfficientGlobalOptimization
R"RAW(Efficient Global Optimization algorithm.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The EGO algorithm [jones1998]_ is an adaptative optimization method based on Gaussian process regression.
An initial design of experiment is used to build a first metamodel.
At each iteration a new point that maximizes a criterion is chosen as optimizer candidate.
The criterion uses a tradeoff between the metamodel value and the conditional variance.
Then the new point is evaluated using the original model and the metamodel is
relearnt on the extended design of experiment.

Parameters
----------
problem : :class:`~openturns.OptimizationProblem`
    The optimization problem to solve
gprResult : :class:`~openturns.GaussianProcessRegressionResult`
    The result of the metamodel on the first design of experiment

Notes
-----
Each point added to the metamodel design seeks to improve the current minimum.
We chose the point so as to maximize an improvement criterion based on the
metamodel.

.. math::

    I(x_{new}) = max(f_{min} - Y_{new}, 0)

The default criteria is called EI (Expected Improvement) and aims at maximizing
the mean improvement:

.. math::

    EI(x_{new}) = \mathbb{E}\left[I(x_{new})\right] = \mathbb{E}\left[max(f_{min} - Y_{new}, 0)\right]

This criterion is made explicit using the Gaussian process mean and variance:

.. math::

    \mathbb{E}\left[I(x_{new})\right] = (f_{min} - m_K(x_{new})) \Phi\left( \frac{f_{min} - m_K(x_{new})}{s_K(x_{new})} \right) + s_K(x_{new}) \phi\left( \frac{f_{min} - m_K(x_{new})}{s_K(x_{new})} \right)

An optional observation heteroskedastic noise variance can be provided through :func:`setNoiseFunction`:

.. math:: Y_{obs} = Y(x) + \sigma_{\epsilon}(x) \epsilon

In that case the Augmented Expected Improvement (AEI) formulation is used.
Because we do not have access to the real minimum of the function in this case, a quantile
of the Gaussian process conditional mean is used, with the constant :math:`c`:

.. math:: u(x) = m_K(x) + c s_K(x)

This criterion is minimized over the design points:

.. math:: x_{min} = \argmax_{x_i} (u(x_i))

The AEI criterion is:

.. math::

    AEI(x_{new}) = \mathbb{E}\left[max(m_K(x_{min}) - Y_{new}, 0)\right] \times \left(1 - \frac{\sigma_{\epsilon}(x_{new})}{\sqrt{\sigma_{\epsilon}^2(x_{new})+s^2_K(x_{new})}} \right)

with

.. math::

    \mathbb{E}\left[max(m_K(x_{min}) - Y_{new}, 0)\right] = (m_K(x_{min}) - m_K(x_{new})) \Phi\left( \frac{m_K(x_{min}) - m_K(x_{new})}{s_K(x_{new})} \right) + s_K(x_{new}) \phi\left( \frac{m_K(x_{min}) - m_K(x_{new})}{s_K(x_{new})} \right)

By default the criteria is maximized using :class:`~openturns.MultiStart`
with starting points uniformly sampled in the optimization problem bounds,
see :func:`setMultiStartExperimentSize` and :func:`setMultiStartNumber`.
This behavior can be overridden by using another solver with :func:`setOptimizationAlgorithm`.

The enrichment criterion itself is a parameter of the algorithm: an
:class:`~openturns.experimental.EfficientGlobalOptimizationCriterion`, the
expected improvement described above by default, replaceable with
:func:`setCriterion` or subclassable from Python. The algorithm keeps the whole
loop: growing design, Gaussian process refit, inner solver and multi-start,
stopping rules and result. A criterion builds the scalar function the inner
solver maximizes, and may declare the convergence of the enrichment through its
own convergence test; :func:`getHasConverged` reports it, and
:func:`getCriterionValues` gives the criterion value at each iteration.

The following :class:`~openturns.ResourceMap` keys are used:

- ``EfficientGlobalOptimization-DefaultAEITradeoff`` (``Scalar``, default: ``1.0``): default value of the tradeoff coefficient of the AEI criterion balancing exploration and exploitation
- ``EfficientGlobalOptimization-DefaultCorrelationLengthFactor`` (``Scalar``, default: ``1.0``): factor used to scale the bounds of the correlation lengths when building the initial experiment of the multiple starts
- ``EfficientGlobalOptimization-DefaultOptimizationAlgorithm`` (``String``, default: ``Cobyla``): name of the optimization algorithm used for the inner maximization of the expected improvement, among the ones recognized by :class:`~openturns.OptimizationAlgorithm`
- ``EfficientGlobalOptimization-DefaultMultiStartExperimentSize`` (``UnsignedInteger``, default: ``100``): size of the design from which the starting points are selected,
- ``EfficientGlobalOptimization-DefaultMultiStartNumber`` (``UnsignedInteger``, default: ``20``): number of retained starting points,
- ``EfficientGlobalOptimization-DefaultParameterEstimationPeriod`` (``UnsignedInteger``, default: ``1``): number of iterations between two estimations of the covariance parameters.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> dim = 4
>>> model = ot.SymbolicFunction(['x1', 'x2', 'x3', 'x4'], ['x1*x1+x2^3*x1+x3+x4'])
>>> model = ot.MemoizeFunction(model)
>>> bounds = ot.Interval([-5.0] * dim, [5.0] * dim)
>>> problem = ot.OptimizationProblem()
>>> problem.setObjective(model)
>>> problem.setBounds(bounds)
>>> inputSample = ot.JointDistribution([ot.Uniform(-5.0, 5.0)] * dim).getSample(30)
>>> outputSample = model(inputSample)
>>> yMin0 = outputSample.getMin()
>>> covarianceModel = ot.SquaredExponential([2.0] * dim, [0.1])
>>> basis = ot.ConstantBasisFactory(dim).build()
>>> fitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
>>> fitter.run()
>>> gpr = ot.GaussianProcessRegression(fitter.getResult())
>>> gpr.run()
>>> algo = otexp.EfficientGlobalOptimization(problem, gpr.getResult())
>>> algo.setMaximumCallsNumber(2)
>>> algo.run()
>>> result = algo.getResult()
>>> updatedGPRResult = algo.getGaussianProcessRegressionResult()
>>> updatedOutputSample = updatedGPRResult.getOutputSample()
>>> yMin = updatedOutputSample.getMin()

)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::setOptimizationAlgorithm
"Expected improvement solver accessor.

Parameters
----------
solver : :class:`~openturns.OptimizationAlgorithm`
    The solver used to optimize the expected improvement"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getOptimizationAlgorithm
"Expected improvement solver accessor.

Returns
-------
solver : :class:`~openturns.OptimizationAlgorithm`
    The solver used to optimize the expected improvement"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::setMultiStartExperimentSize
"Size of the design to draw starting points.

Parameters
----------
multiStartExperimentSize : int
    The size of the Monte Carlo design from which to select the best starting
    points.
    The default number can be tweaked with the
    ``EfficientGlobalOptimization-DefaultMultiStartExperimentSize`` key from
    :class:`~openturns.ResourceMap`."

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getMultiStartExperimentSize
"Size of the design to draw starting points.

Returns
-------
multiStartExperimentSize : int
    The size of the Monte Carlo design from which to select the best starting
    points."

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::setMultiStartNumber
"Number of starting points for the criterion optimization.

Parameters
----------
multiStartNumber : int
    The number of starting points for the criterion optimization.
    The default number can be tweaked with the
    ``EfficientGlobalOptimization-DefaultMultiStartNumber`` key from
    :class:`~openturns.ResourceMap`."

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getMultiStartNumber
"Number of starting points for the criterion optimization.

Returns
-------
multiStartNumber : int
    The number of starting points for the criterion optimization."

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::setParameterEstimationPeriod
"Parameter estimation period accessor.

Parameters
----------
period : int
    The number of iterations between covariance parameters re-learn.
    Default is 1 (each iteration). Can be set to 0 (never).
    The default number can be tweaked with the
    ``EfficientGlobalOptimization-DefaultParameterEstimationPeriod`` key from
    :class:`~openturns.ResourceMap`."

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getParameterEstimationPeriod
"Parameter estimation period accessor.

Returns
-------
period : int
    The number of iterations between covariance parameters re-learn.
    Default is 1 (each iteration). Can be set to 0 (never)."

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::setCorrelationLengthFactor
R"RAW(Correlation length stopping criterion factor accessor.

When a correlation length becomes smaller than the minimal distance between
design point for a single component that means the model tends to be noisy,
and the EGO formulation is not adapted anymore.

Parameters
----------
b : float
    Used to define a stopping criterion on the minimum correlation length:
    :math:`\theta_i < \frac{\Delta_i^{min}}{b}`
    with :math:`\Delta^{min}` the minimum distance between design points.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getCorrelationLengthFactor
R"RAW(Correlation length stopping criterion factor accessor.

When a correlation length becomes smaller than the minimal distance between
design point for a single component that means the model tends to be noisy,
and the EGO formulation is not adapted anymore.

Returns
-------
b : float
    Used to define a stopping criterion on the minimum correlation length:
    :math:`\theta_i < \frac{\Delta_i^{min}}{b}`
    with :math:`\Delta^{min}` the minimum distance between design points.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::setAEITradeoff
"AEI tradeoff constant accessor.

Parameters
----------
c : float
    Used to define a quantile of the Gaussian process prediction at the design points.
    :math:`u(x)=m_K(x)+c*s_K(x)`"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getAEITradeoff
"AEI tradeoff constant accessor.

Returns
-------
c : float
    Used to define a quantile of the Gaussian process prediction at the design points.
    :math:`u(x)=m_K(x)+c*s_K(x)`"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getExpectedImprovement
"Value of the criterion at each iteration.

.. deprecated:: 1.28
    Kept for compatibility: with a pluggable criterion these values are no
    longer necessarily an expected improvement. Use :func:`getCriterionValues`.

Returns
-------
values : :class:`~openturns.Sample`
    The optimal values of the criterion, one per iteration."

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::setCriterion
R"RAW(Enrichment criterion accessor.

Parameters
----------
criterion : :class:`~openturns.experimental.EfficientGlobalOptimizationCriterion`
    The criterion whose objective is maximized at each iteration.

Notes
-----
The criterion is an :class:`~openturns.experimental.ExpectedImprovement` until
this method is called. The noise function set with :func:`setNoiseFunction` is
propagated to the new criterion, so a criterion replaced after the noise has
been set still sees it.

The algorithm keeps a private copy: the state it pushes into the criterion at
each iteration does not modify the object passed here.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getCriterion
R"RAW(Enrichment criterion accessor.

Returns
-------
criterion : :class:`~openturns.experimental.EfficientGlobalOptimizationCriterion`
    The criterion whose objective is maximized at each iteration.

Notes
-----
The returned handle shares its implementation with the algorithm: changing one
of its parameters changes the criterion the algorithm uses. This is how
:func:`setAEITradeoff` and :func:`setNoiseFunction` reach the default
criterion.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getCriterionValues
"Value of the criterion at each iteration.

Returns
-------
values : :class:`~openturns.Sample`
    The optimal values of the criterion, one per iteration."

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getHasConverged
"Whether the loop ended on the convergence test of the criterion.

Returns
-------
hasConverged : bool
    Whether the enrichment was ended by the criterion convergence test, as
    opposed to the call budget, the correlation length criterion or a user
    callback."

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::setNoiseFunction
R"RAW(Noise model accessor.

Parameters
----------
noiseFunction : :class:`~openturns.Function`
    The noise variance function :math:`\sigma^2_{\epsilon}(x)` used for the AEI criterion.
    Of same input dimension as the objective and 1-d output.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getNoiseFunction
R"RAW(Noise model accessor.

Returns
-------
noiseFunction : :class:`~openturns.Function`
    The noise variance function :math:`\sigma^2_{\epsilon}(x)` used for the AEI criterion.
    Of same input dimension as the objective and 1-d output.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getGaussianProcessRegressionResult
"Retrieve the GPR result.

Notes
-----
Before :meth:`run` is called, this method returns the result passed to the constructor.
Once :meth:`run` has been called, it returns an updated result that takes new observations into account.

Returns
-------
gprResult : :class:`~openturns.GaussianProcessRegressionResult`
    Result that takes all observations into account."
