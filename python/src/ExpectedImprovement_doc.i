%feature("docstring") OT::ExpectedImprovement
R"RAW(Expected improvement criterion of the EGO algorithm.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This is the historical enrichment rule of
:class:`~openturns.experimental.EfficientGlobalOptimization`, and its default
criterion.

Without a noise function, the reference level is the best objective value
observed so far, and the criterion is the expected improvement of the Gaussian
process over that value.

With a noise function, the true best value is unknown: the reference level is a
quantile of the conditional prediction over the current design, the minimum (for
a minimization) or the maximum (for a maximization) of
:math:`m(x_i) + c\,s(x_i)`, where :math:`c` is the AEI tradeoff. The expected
improvement is then scaled by the fraction of the predictive variance that is
not noise: this is the augmented expected improvement.

Parameters
----------
aeiTradeoff : float, optional
    Tradeoff constant :math:`c` of the augmented expected improvement, used
    only when a noise function is set. Defaults to the
    :class:`~openturns.ResourceMap` key
    `EfficientGlobalOptimization-DefaultAEITradeoff`.

See Also
--------
EfficientGlobalOptimization, EfficientGlobalOptimizationCriterion
)RAW"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ExpectedImprovement::setAEITradeoff
"AEI tradeoff constant accessor.

Parameters
----------
aeiTradeoff : float
    Tradeoff constant :math:`c` of the quantile :math:`m(x_i) + c\\,s(x_i)` used
    as the reference level when the objective is noisy."

// ---------------------------------------------------------------------------

%feature("docstring") OT::ExpectedImprovement::getAEITradeoff
"AEI tradeoff constant accessor.

Returns
-------
aeiTradeoff : float
    Tradeoff constant :math:`c` of the quantile :math:`m(x_i) + c\\,s(x_i)` used
    as the reference level when the objective is noisy."

// ---------------------------------------------------------------------------

%feature("docstring") OT::ExpectedImprovement::buildObjective
"Build the expected improvement, to be maximized by the algorithm.

Returns
-------
objective : :class:`~openturns.Function`
    Expected improvement of the current Gaussian process over the reference
    level, augmented when a noise function is set."
