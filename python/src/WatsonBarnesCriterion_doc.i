%feature("docstring") OT::WatsonBarnesCriterion
R"RAW(Second Watson and Barnes infill criterion (WB2) of the EGO algorithm: the
expected improvement over the best value observed so far, summed with the
negative conditional mean of the Gaussian process, so that a candidate is
rewarded both for being expected to improve and for being predicted low.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

For a minimization, with :math:`m_K` and :math:`s_K` the conditional mean and
standard deviation of the Gaussian process fitted on the current design,
:math:`x_{min}` the point where the best value has been observed, and
:math:`\Phi`, :math:`\phi` the cumulative distribution function and the density
of the standard normal distribution,

.. math::

    \mathrm{WB2}(x) = -m_K(x) + \Delta(x)\,\Phi\left(\frac{\Delta(x)}{s_K(x)}\right) + s_K(x)\,\phi\left(\frac{\Delta(x)}{s_K(x)}\right)

with :math:`\Delta(x) = m_K(x_{min}) - m_K(x)`, and the mirror
:math:`\mathrm{WB2}(x) = m_K(x) + \ldots` with :math:`\Delta(x) = m_K(x) -
m_K(x_{max})` for a maximization: the algorithm always maximizes the objective
returned by :meth:`~openturns.experimental.EfficientGlobalOptimizationCriterion.buildObjective`.
The criterion is the second infill rule of Watson and Barnes (1995); the
expected improvement it adds to the predicted value is that of Jones et al.
(1998).

The difficulty of the criterion is the balance of its two terms, summed with a
fixed weight of one: both carry the scale of the objective, but their
magnitudes are not related to each other, the expected improvement being
bounded by the dispersion of the surrogate while the predicted-value term
spans the whole range of the objective. The balance between exploitation and
exploration therefore depends on the problem rather than on the surrogate, and
cannot be tuned: that is the difficulty the scaled variant (WB2S) addresses, by
multiplying the expected improvement by a factor computed from its maximum over
the domain found by a multi-start.

At the measured points, where the predictive variance vanishes, the expected
improvement is zero as for :class:`~openturns.experimental.ExpectedImprovement`,
so the criterion is there the predicted-value term alone and a measured point
can win the maximization.

See Also
--------
EfficientGlobalOptimization, EfficientGlobalOptimizationCriterion, ExpectedImprovement, ProbabilityImprovement, LowerConfidenceBound
)RAW"

// ---------------------------------------------------------------------------

%feature("docstring") OT::WatsonBarnesCriterion::buildObjective
R"RAW(Build the Watson and Barnes criterion, to be maximized by the algorithm.

Returns
-------
objective : :class:`~openturns.Function`
    Expected improvement over the best value observed so far, summed with the
    opposite of the conditional mean in a minimization.

Notes
-----
The criterion is defined for a noiseless objective: the reference level is the
best value ``getOptimalValue()`` observed on the design, which a noise model
does not provide. Setting a noise function raises ``NotYetImplementedException``;
the augmented expected improvement, :class:`~openturns.experimental.ExpectedImprovement`
with a noise function, is the established noisy rule.)RAW"
