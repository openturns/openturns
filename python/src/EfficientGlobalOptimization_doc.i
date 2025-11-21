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
gprResult : :class:`~openturns.experimental.GaussianProcessRegressionResult`
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

This criterion is explicited using the Gaussian process mean and variance:

.. math::

    \mathbb{E}\left[I(x_{new})\right] = (f_{min} - m_K(x_{new})) \Phi\left( \frac{f_{min} - m_K(x_{new})}{s_K(x_{new})} \right) + s_K(x_{new}) \phi\left( \frac{f_{min} - m_K(x_{new})}{s_K(x_{new})} \right)

In case the covariance model of the Gaussian process model has a non-zero nugget factor :math:`\sigma_{\epsilon}`
(i.e. the emulated function is noisy) the AEI (Augmented Expected Improvement) formulation is used.
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

By default the criteria is minimized using :class:`~openturns.MultiStart`
with starting points uniformly sampled in the optimization problem bounds,
see :func:`setMultiStartExperimentSize` and :func:`setMultiStartNumber`.
This behavior can be overridden by using another solver with :func:`setOptimizationAlgorithm`.

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
>>> fitter = otexp.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
>>> fitter.run()
>>> gpr = otexp.GaussianProcessRegression(fitter.getResult())
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
    `EfficientGlobalOptimization-DefaultMultiStartExperimentSize` key from
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
    `EfficientGlobalOptimization-DefaultMultiStartNumber` key from
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
    `EfficientGlobalOptimization-DefaultParameterEstimationPeriod` key from
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
"Expected improvement values.

Returns
-------
ei : :class:`~openturns.Sample`
    The expected improvement optimal values."

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::setMetamodelNoise
R"RAW(Metamodel noise model accessor.

Parameters
----------
noiseModel : :class:`~openturns.Function`
    The noise variance function :math:`\sigma^2_{\epsilon}(x)` used for the AEI
    design update only.
    Of same input dimension as the objective and 1-d output.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getMetamodelNoise
R"RAW(Metamodel noise model accessor.

Returns
-------
noiseModel : :class:`~openturns.Function`
    The noise variance function :math:`\sigma^2_{\epsilon}(x)` used for the AEI
    design update only.
    Of same input dimension as the objective and 1-d output.)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::setNoiseModel
R"RAW(Improvement noise model accessor.

Parameters
----------
noiseModel : :class:`~openturns.Function`
    The noise variance function :math:`\sigma^2_{\epsilon}(x)` used for the AEI
    criterion optimization only.
    Of same input dimension as the objective and 1-d output.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimization::getNoiseModel
R"RAW(Improvement noise model accessor.

Returns
-------
noiseModel : :class:`~openturns.Function`
    The noise variance function :math:`\sigma^2_{\epsilon}(x)` used for the AEI
    criterion optimization only.
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
gprResult : :class:`~openturns.experimental.GaussianProcessRegressionResult`
    Result that takes all observations into account."
