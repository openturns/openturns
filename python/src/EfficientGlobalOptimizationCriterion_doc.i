%feature("docstring") OT::EfficientGlobalOptimizationCriterion
R"RAW(Base class of the enrichment criterion used by :class:`~openturns.experimental.EfficientGlobalOptimization`.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class is the handle of the enrichment criterion hierarchy. It carries the
criterion by value with value semantics, so it can be stored in and returned
from the algorithm, saved in a study and subclassed from Python.

A criterion only answers two questions, at each iteration of the algorithm:
which scalar function must the inner solver maximize, and whether the criterion
value at the new point ends the enrichment. The algorithm keeps everything else:
the growing design of experiments, the Gaussian process refit, the inner solver
and its multi-start, the stopping rules and the result.

The default criterion is the expected improvement,
:class:`~openturns.experimental.ExpectedImprovement`. A criterion can be
replaced with :meth:`~openturns.experimental.EfficientGlobalOptimization.setCriterion`.

See Also
--------
EfficientGlobalOptimization, ExpectedImprovement, PythonEfficientGlobalOptimizationCriterion
)RAW"

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::setGaussianProcessRegression
"Gaussian process surrogate accessor.

Parameters
----------
gprResult : :class:`~openturns.GaussianProcessRegressionResult`
    Current Gaussian process regression result."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::getGaussianProcessRegression
"Gaussian process surrogate accessor.

Returns
-------
gprResult : :class:`~openturns.GaussianProcessRegressionResult`
    Current Gaussian process regression result."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::setInputSample
"Current design of experiments accessor.

Parameters
----------
inputSample : :class:`~openturns.Sample`
    Input sample of the current design of experiments."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::getInputSample
"Current design of experiments accessor.

Returns
-------
inputSample : :class:`~openturns.Sample`
    Input sample of the current design of experiments."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::setOptimalValue
"Best objective value observed so far accessor.

Parameters
----------
optimalValue : float
    Best value of the objective observed on the design so far."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::getOptimalValue
"Best objective value observed so far accessor.

Returns
-------
optimalValue : float
    Best value of the objective observed on the design so far."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::setMinimization
"Direction of the problem accessor.

Parameters
----------
minimization : bool
    Whether the problem is a minimization."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::getMinimization
"Direction of the problem accessor.

Returns
-------
minimization : bool
    Whether the problem is a minimization."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::setNoiseFunction
"Noise variance function accessor.

Parameters
----------
noiseFunction : :class:`~openturns.Function`
    Variance of the observation noise, empty when the objective is noiseless."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::getNoiseFunction
"Noise variance function accessor.

Returns
-------
noiseFunction : :class:`~openturns.Function`
    Variance of the observation noise, empty when the objective is noiseless."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::hasNoiseFunction
"Whether a noise function was provided.

Returns
-------
hasNoise : bool
    Whether the objective is noisy."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::buildObjective
"Build the objective the inner solver of the algorithm maximizes.

Returns
-------
objective : :class:`~openturns.Function`
    A scalar function defined on the problem domain, maximized by the algorithm."

// ---------------------------------------------------------------------------

%feature("docstring") OT::EfficientGlobalOptimizationCriterion::checkConvergence
"Convergence test on the criterion value reached at the new point.

Parameters
----------
criterionValue : float
    Value of the criterion at the new point.

Returns
-------
hasConverged : bool
    Whether the enrichment is over. The default never stops."
