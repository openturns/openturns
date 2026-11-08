%feature("docstring") OT::SequentialQuantileSimulation
R"RAW(Sequential quantile simulation with an anytime-valid confidence sequence.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    SequentialQuantileSimulation(*output, alpha=0.95, beta=0.95*)

The algorithm maintains a distribution-free confidence band for the
distribution function :math:`F` from the
Dvoretzky-Kiefer-Wolfowitz-Massart inequality
[dvoretzky1956]_ [massart1990]_,

.. math::

    \mathbb{P}\left(
      \sup_x \left|\hat F_n(x) - F(x)\right| > \varepsilon_n
    \right) \le 2 e^{-2n\varepsilon_n^2},

and evaluates it on a deterministic geometric grid of candidate sample
sizes.  At grid index :math:`k` the failure probability is

.. math::

    \delta_k = \frac{1-\beta}{(k+1)(k+2)},
    \qquad
    \varepsilon_k = \sqrt{
      \frac{\log(2/\delta_k)}{2n_k}
    }.

Because :math:`\sum_{k\ge 0}\delta_k = 1-\beta`, the intervals hold
simultaneously at every candidate size with probability at least
:math:`\beta`.  This union bound is what makes the interval valid at
the data-dependent stopping time; a fixed-sample order-statistic
interval evaluated at a random stopping time would not be.

The band is inverted into a quantile interval by

.. math::

    q_\alpha \in
    \left[
      \inf\{x : \hat F_n(x) \ge \alpha - \varepsilon_n\},
      \inf\{x : \hat F_n(x) > \alpha + \varepsilon_n\}
    \right],

which is evaluated with the order statistics of the sample.  An
endpoint is infinite when :math:`\alpha \pm \varepsilon_n` leaves
:math:`(0,1)`; the result reports the corresponding infinite bound.

The algorithm stops when the interval half-width reaches the target set
by :meth:`setTargetHalfWidth`, or when the evaluation budget set by
:meth:`setSampleSize` is reached.  Without a target it always runs to
the budget.  The stopping time and the stopping criterion are reported
by :meth:`QuantileSimulationResult.getStoppingTime` and
:meth:`QuantileSimulationResult.getStoppingCriterion`, and the full path
is available through
:meth:`QuantileSimulationResult.getConfidenceSequencePath`.  If the
budget is reached before the target is met, the interval is reported at
the stopping time,
:meth:`QuantileSimulationResult.getBudgetExhausted` is set to ``True``
and a warning is logged.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
alpha : float
    Quantile level, in :math:`(0, 1)`.  Default is 0.95.
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.
    Default is 0.95.

Notes
-----
The coverage is distribution-free and anytime-valid: it requires
independent and identically distributed observations and does not
assume a bounded or sub-Gaussian output.  The price is a conservative
interval.  The tighter Howard-Ramdas confidence sequence is not
implemented; the result records the Dvoretzky-Kiefer-Wolfowitz-Massart
assumption in :meth:`QuantileSimulationResult.getCoverageAssumption`.

The following :class:`~openturns.ResourceMap` keys are used:

- ``SequentialQuantileSimulation-DefaultTargetHalfWidth`` (``Scalar``, default: ``0.0``): default target half-width; zero means that no target is set and the run stops at the budget.
- ``SequentialQuantileSimulation-GridRatio`` (``Scalar``, default: ``1.2``): ratio between consecutive candidate sample sizes; must be larger than one.
- ``SequentialQuantileSimulation-MinimumGridSize`` (``UnsignedInteger``, default: ``64``): first candidate sample size.

The achieved coverage is ``COVERAGE_ANYTIME_VALID`` and
:meth:`QuantileSimulationResult.isFixedSample` is ``False``, because the
stopping time is data-dependent.

See also
--------
TwoStageQuantileSimulation, QuantileSimulationAlgorithm, QuantileSimulationResult

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.RandomVector(ot.Normal())
>>> algo = otexp.SequentialQuantileSimulation(output, 0.95, 0.95)
>>> algo.setSampleSize(100000)
>>> algo.setTargetHalfWidth(0.2)
>>> algo.run()
>>> result = algo.getResult()
>>> stoppingTime = result.getStoppingTime()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SequentialQuantileSimulation::getTargetHalfWidth
R"RAW(Accessor to the target half-width.

Returns
-------
targetHalfWidth : float
    Target half-width of the confidence sequence, in the output units.
    Zero means that no target is set.  Default is 0.0.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SequentialQuantileSimulation::setTargetHalfWidth
R"RAW(Accessor to the target half-width.

Parameters
----------
targetHalfWidth : float
    Target half-width of the confidence sequence, in the output units.
    It must be finite and positive.  Default is 0.0, which means that
    no target is set and the run stops at the evaluation budget.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SequentialQuantileSimulation::hasTargetHalfWidth
R"RAW(Test whether a target half-width is set.

Returns
-------
hasTargetHalfWidth : bool
    Whether :meth:`setTargetHalfWidth` has been called with a positive
    value.)RAW"
