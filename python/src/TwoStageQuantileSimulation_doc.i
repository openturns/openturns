%feature("docstring") OT::TwoStageQuantileSimulation
R"RAW(Two-stage quantile simulation with an exact confirmation interval.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    TwoStageQuantileSimulation(*output, alpha=0.95, beta=0.95*)

The two-stage design solves the optional-stopping problem without an
anytime-valid confidence sequence.  A pilot sample of size :math:`N_0`
first gives the exact bilateral order-statistic interval
:math:`[X_{(j_1)}, X_{(j_2)}]` for the quantile; let :math:`h_0` be its
half-width.  The confirmation size is then chosen so that the expected
half-width of the confirmation interval reaches the target :math:`h`
set by :meth:`setTargetHalfWidth`:

.. math::

    N_1 = \left\lceil N_0 \left(\frac{h_0}{h}\right)^2 \right\rceil.

This is the finite-sample form of the asymptotic rule

.. math::

    N_1 \approx \left\lceil
      \frac{z_{1-\beta/2}^2\,\alpha(1-\alpha)}
           {f(q_\alpha)^2\,h^2}
    \right\rceil,

where :math:`z_{1-\beta/2}` is the standard normal quantile.  Using
the pilot interval width instead of a single-spacing density estimate
avoids the large variance of the latter.  The spacing estimator

.. math::

    \hat f(q_\alpha) = \frac{j_2 - j_1}
    {N_0 \left(X_{(j_2)} - X_{(j_1)}\right)}

is still reported by :meth:`QuantileSimulationResult.getPilotDensity`.
When the pilot is too small for an exact interval, the density estimate
is used with the asymptotic rule instead.  The size is clamped from
below by the minimum sample size of the exact bilateral
order-statistic interval and from above by the remaining evaluation
budget.

The confirmation sample is drawn independently of the pilot.  The
estimate is the empirical inverse of the confirmation sample and the
confidence interval is the exact order-statistic interval
:math:`[X_{(k_1)},X_{(k_2)}]` computed on that sample only.  Because
:math:`N_1` is fixed before the confirmation sample is drawn, and is a
function of the independent pilot sample, the exact binomial coverage
holds unconditionally, pilot randomness included.  The pilot
observations are not used in the estimate or in the interval.

The evaluation budget is set with :meth:`setSampleSize` and is shared by
the two stages.  If the budget is insufficient to reach the target
half-width, the confirmation sample is limited to the available budget,
:meth:`QuantileSimulationResult.getBudgetExhausted` is set to ``True``
and a warning is logged.  The interval remains exact at the achieved
confirmation size; only the target width is missed.

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
The following :class:`~openturns.ResourceMap` keys are used:

- ``TwoStageQuantileSimulation-DefaultPilotSize`` (``UnsignedInteger``, default: ``1000``): default pilot sample size.
- ``TwoStageQuantileSimulation-DefaultTargetHalfWidth`` (``Scalar``, default: ``0.0``): default target half-width; zero means that no target is set and the confirmation stage uses the remaining budget.

The achieved coverage is ``COVERAGE_EXACT`` and
:meth:`QuantileSimulationResult.isFixedSample` is ``True``, because the
confirmation size is fixed before the confirmation sample is drawn.

See also
--------
SequentialQuantileSimulation, QuantileSimulationAlgorithm, QuantileSimulationResult

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.RandomVector(ot.Normal())
>>> algo = otexp.TwoStageQuantileSimulation(output, 0.95, 0.95)
>>> algo.setSampleSize(3000)
>>> algo.setPilotSize(500)
>>> algo.setTargetHalfWidth(0.1)
>>> algo.run()
>>> result = algo.getResult()
>>> estimate = result.getQuantileEstimate()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::TwoStageQuantileSimulation::getPilotSize
R"RAW(Accessor to the pilot sample size.

Returns
-------
pilotSize : int
    Number of observations drawn in the pilot stage.  The pilot is
    capped so that the remaining budget can hold an exact confirmation
    interval.  Default is 1000.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::TwoStageQuantileSimulation::setPilotSize
R"RAW(Accessor to the pilot sample size.

Parameters
----------
pilotSize : int
    Number of observations drawn in the pilot stage.  It must be
    positive.  Default is 1000.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::TwoStageQuantileSimulation::getTargetHalfWidth
R"RAW(Accessor to the target half-width.

Returns
-------
targetHalfWidth : float
    Target half-width of the confirmation interval, in the output
    units.  Zero means that no target is set.  Default is 0.0.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::TwoStageQuantileSimulation::setTargetHalfWidth
R"RAW(Accessor to the target half-width.

Parameters
----------
targetHalfWidth : float
    Target half-width of the confirmation interval, in the output
    units.  It must be finite and positive.  Default is 0.0, which
    means that no target is set.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::TwoStageQuantileSimulation::hasTargetHalfWidth
R"RAW(Test whether a target half-width is set.

Returns
-------
hasTargetHalfWidth : bool
    Whether :meth:`setTargetHalfWidth` has been called with a positive
    value.)RAW"
