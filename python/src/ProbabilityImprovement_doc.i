%feature("docstring") OT::ProbabilityImprovement
R"RAW(Probability of improvement criterion of the EGO algorithm: the
probability, under the current Gaussian process, that a new evaluation of the
objective beats the best value observed so far.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

For a minimization, with :math:`m_K` and :math:`s_K` the conditional mean and
standard deviation of the Gaussian process fitted on the current design,
:math:`x_{min}` the point where the best value has been observed, and
:math:`\Phi` the cumulative distribution function of the standard normal
distribution,

.. math::

    \mathrm{PI}(x) = \Phi\left(\frac{m_K(x_{min}) - m_K(x)}{s_K(x)}\right)

and the mirror :math:`\Phi\left(\frac{m_K(x) - m_K(x_{max})}{s_K(x)}\right)`
for a maximization.

It is the expected improvement without the amount of the improvement: the
expected improvement is the first moment of the improvement random variable,
the probability of improvement only states that its sign is favorable. The
criterion was proposed by Kushner (1964) as an infill rule for response surface
models, and its use within efficient global optimization is described by Jones
et al. (1998).

The difficulty of the criterion is saturation: :math:`\Phi` tends to one as
soon as the improvement margin is a couple of standard deviations above zero,
which leaves a wide plateau for the inner solver to rank; and large predictive
deviations drive it to :math:`\Phi(0) = 1/2` instead of above, so the rule is
dominated by exploitation. For a differentiable covariance model, the
conditional standard deviation vanishes at a measured point at the same order
as the improvement margin does, so their ratio tends to a positive constant on
the side where the surrogate predicts a better value than the best one
observed: the maximum of the criterion is approached at the measured point of
the best value, and the enrichment concentrates there, adding points
arbitrarily close to an already evaluated one. The literature usually
multiplies the criterion by an improvement factor to break that plateau; the
plain criterion is implemented here, as the benchmark definition states it.

At the measured points, where the predictive variance vanishes, the ratio is
an indeterminate form and the objective is set to zero, as for the expected
improvement.

See Also
--------
EfficientGlobalOptimization, EfficientGlobalOptimizationCriterion, ExpectedImprovement
)RAW"

// ---------------------------------------------------------------------------

%feature("docstring") OT::ProbabilityImprovement::buildObjective
R"RAW(Build the probability of improvement, to be maximized by the algorithm.

Returns
-------
objective : :class:`~openturns.Function`
    Probability, under the current Gaussian process, that a new evaluation
    beats the best value observed so far.

Notes
-----
The criterion is defined for a noiseless objective: the reference level is the
best value ``getOptimalValue()`` observed on the design, which a noise model
does not provide. Setting a noise function raises ``NotYetImplementedException``;
the augmented expected
improvement, :class:`~openturns.experimental.ExpectedImprovement` with a noise
function, is the established noisy rule.)RAW"
