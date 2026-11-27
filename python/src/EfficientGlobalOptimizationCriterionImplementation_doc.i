%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation
R"RAW(Base class of the enrichment criterion used by :class:`~openturns.experimental.EfficientGlobalOptimization`.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The algorithm owns the enrichment loop: it grows the design of experiments,
refits the Gaussian process, maximizes the objective returned by
:meth:`buildObjective` with its inner solver, evaluates the true model at the
new point and applies its stopping rules. A criterion only answers two
questions, at each iteration:

- which scalar function must the inner solver maximize?
- does the criterion value at the new point end the enrichment?

Before each call to :meth:`buildObjective`, the algorithm pushes its current
state into the criterion with :meth:`setGaussianProcessRegression`,
:meth:`setInputSample`, :meth:`setOptimalValue`, :meth:`setMinimization` and
:meth:`setNoiseFunction`, so an implementation never has to guess which
iteration it is serving.

The objective returned by :meth:`buildObjective` is always *maximized*, whatever
the direction of the problem: a criterion built for a minimization returns the
opposite of its quantity of interest.

Subclass :class:`~openturns.experimental.PythonEfficientGlobalOptimizationCriterion`
to define a criterion in Python.

See Also
--------
EfficientGlobalOptimization, ExpectedImprovement
)RAW"

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::setGaussianProcessRegression
"Gaussian process surrogate accessor.

The algorithm sets it to the surrogate of the current iteration before each
call to :meth:`buildObjective`.

Parameters
----------
gprResult : :class:`~openturns.GaussianProcessRegressionResult`
    Current Gaussian process regression result."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::getGaussianProcessRegression
"Gaussian process surrogate accessor.

Returns
-------
gprResult : :class:`~openturns.GaussianProcessRegressionResult`
    Current Gaussian process regression result."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::setInputSample
"Current design of experiments accessor.

Parameters
----------
inputSample : :class:`~openturns.Sample`
    Input sample of the design of experiments of the current iteration."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::getInputSample
"Current design of experiments accessor.

Returns
-------
inputSample : :class:`~openturns.Sample`
    Input sample of the design of experiments of the current iteration."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::setOptimalValue
"Best objective value observed so far accessor.

Parameters
----------
optimalValue : float
    Best value of the objective observed on the design so far."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::getOptimalValue
"Best objective value observed so far accessor.

Returns
-------
optimalValue : float
    Best value of the objective observed on the design so far."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::setMinimization
"Direction of the problem accessor.

Parameters
----------
minimization : bool
    Whether the problem is a minimization. The criterion built by
    :meth:`buildObjective` is maximized in both cases."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::getMinimization
"Direction of the problem accessor.

Returns
-------
minimization : bool
    Whether the problem is a minimization."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::setNoiseFunction
"Noise variance function accessor.

Parameters
----------
noiseFunction : :class:`~openturns.Function`
    Variance of the observation noise, empty when the objective is noiseless."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::getNoiseFunction
"Noise variance function accessor.

Returns
-------
noiseFunction : :class:`~openturns.Function`
    Variance of the observation noise, empty when the objective is noiseless."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::hasNoiseFunction
"Whether a noise function was provided.

Returns
-------
hasNoise : bool
    Whether the objective is noisy."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::buildObjective
"Build the objective the inner solver of the algorithm maximizes.

Returns
-------
objective : :class:`~openturns.Function`
    A scalar function defined on the problem domain, whose input dimension is
    the dimension of the problem. The algorithm maximizes it, and checks both
    dimensions before running its solver."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterionImplementation::checkConvergence
"Convergence test on the criterion value reached at the new point.

Called once the new point has been measured and added to the design. The
default never stops.

Parameters
----------
criterionValue : float
    Value of the criterion at the new point.

Returns
-------
hasConverged : bool
    Whether the enrichment is over."
