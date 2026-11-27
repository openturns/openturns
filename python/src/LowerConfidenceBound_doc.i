%feature("docstring") OT::LowerConfidenceBound
R"RAW(Lower confidence bound criterion of the EGO algorithm: a pessimistic
estimate of the objective, the conditional mean of the Gaussian process lowered
by a multiple of its conditional standard deviation.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The criterion

.. math::

    \mathrm{LCB}(x) = m_K(x) - k\,s_K(x)

is minimized, where :math:`m_K` and :math:`s_K` are the conditional mean and
standard deviation of the Gaussian process fitted on the current design, and
:math:`k` the conservative factor relative to the standard deviation. Because
the algorithm always maximizes the objective returned by
:meth:`~openturns.experimental.EfficientGlobalOptimizationCriterion.buildObjective`,
a minimization uses :math:`-m_K(x) + k\,s_K(x)`, and a maximization its mirror
:math:`m_K(x) + k\,s_K(x)`: the recorded criterion value is the negated
confidence bound, not an improvement. The bound is the infill rule of Cox and
John (1992).

The conservative factor is the whole exploration/exploitation tradeoff in a
single number: a small factor turns the criterion into a plain predicted value
rule, a large one into a variance rule. Unlike the expected improvement, the
bound does not use the best value observed so far, so its value lives on the
scale of the objective and is not comparable across criteria or across
iterations; the stopping rules therefore stay with the algorithm, and the
criterion declares no convergence.

Parameters
----------
conservativeFactor : float, optional
    Factor :math:`k` of the standard deviation in :math:`m_K(x) - k\,s_K(x)`.
    Must be nonnegative. Defaults to the :class:`~openturns.ResourceMap` key
    ``LowerConfidenceBound-DefaultConservativeFactor``.

See Also
--------
EfficientGlobalOptimization, EfficientGlobalOptimizationCriterion, ExpectedImprovement, ProbabilityImprovement

Notes
-----
The following :class:`~openturns.ResourceMap` key is used:

- ``LowerConfidenceBound-DefaultConservativeFactor`` (``Scalar``, default: ``1.0``): default factor of the standard deviation in the bound

.. warning::
    A noise function must not be set: the bound is computed on the objective
    values of the design, and it is not established whether, with a noise
    model, it should bound the observed values - where the predictive
    deviation already contains the noise - or the true objective, which would
    need a noise variance discount. Setting one raises
    ``NotYetImplementedException``; the augmented expected improvement,
    :class:`~openturns.experimental.ExpectedImprovement` with a noise function,
    is the established noisy rule.)RAW"

// ---------------------------------------------------------------------------

%feature("docstring") OT::LowerConfidenceBound::setConservativeFactor
"Conservative factor accessor.

Parameters
----------
conservativeFactor : float
    Factor :math:`k` of the standard deviation in :math:`m_K(x) - k\\,s_K(x)`.
    Must be finite and nonnegative."

// ---------------------------------------------------------------------------

%feature("docstring") OT::LowerConfidenceBound::getConservativeFactor
"Conservative factor accessor.

Returns
-------
conservativeFactor : float
    Factor :math:`k` of the standard deviation in :math:`m_K(x) - k\\,s_K(x)`."

// ---------------------------------------------------------------------------

%feature("docstring") OT::LowerConfidenceBound::buildObjective
R"RAW(Build the confidence bound to be maximized by the algorithm.

Returns
-------
objective : :class:`~openturns.Function`
    :math:`-m_K(x) + k\,s_K(x)` for a minimization, :math:`m_K(x) + k\,s_K(x)`
    for a maximization.)RAW"
