%feature("docstring") OT::ExactTailQuantileSimulation
R"RAW(Exact-tail quantile simulation.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    ExactTailQuantileSimulation(*output, alpha=0.95, beta=0.95*)

    ExactTailQuantileSimulation(*output, experiment, alpha=0.95, beta=0.95*)

The exact-tail estimator computes an empirical quantile and its exact
binomial interval without storing the full sample.  For a sample of
size :math:`N` and a distribution function continuous at
:math:`q_\alpha`, let :math:`B \sim \Bin(N, \alpha)`
and let :math:`0 \le r_1 < r_2 < N` be the zero-based ranks returned by
:class:`~openturns.QuantileConfidence` for the levels :math:`\alpha`
and :math:`\beta`.  With the one-based order statistics
:math:`Y_{(1)} \le \dots \le Y_{(N)}`, the interval
:math:`[Y_{(r_1+1)}, Y_{(r_2+1)}]` has the exact distribution-free
coverage :math:`\Prob{r_1 + 1 \le B \le r_2} \ge \beta`.  If :math:`F`
has an atom at :math:`q_\alpha`, the binomial parameter is
:math:`F(q_\alpha)\ge\alpha` and the coverage is conservative.  The
point estimate is the empirical inverse
:math:`Y_{(\lceil N\alpha \rceil)}`.

The ranks are computed before sampling.  For an upper-tail quantile
only the :math:`N - r_1` largest observations are needed, and for a
lower-tail quantile only the :math:`r_2 + 1` smallest observations are
needed.  The estimator keeps the smaller of the two tails in a bounded
heap and updates it chunk by chunk, so its memory is bounded by the
retained tail plus one chunk of input and output values.  The estimate and the interval are exactly the order
statistics of the full sample, with no approximation.  The derivation,
the coverage formula and the memory analysis are presented in
:any:`quantile_theory_block`.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
experiment : :class:`~openturns.WeightedExperiment`
    Independent design of experiments with uniform weights.  Its
    distribution must match the input distribution of ``output``.
    When omitted, a :class:`~openturns.MonteCarloExperiment` is built
    from the distribution of ``output``.
alpha : float
    Quantile level, in :math:`(0, 1)`.  Default is 0.95.
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.
    Default is 0.95.

See also
--------
MonteCarloQuantileSimulation, BlockQuantileSimulation, QuantileSimulationResult

Notes
-----
The estimator is intended for extreme quantile levels, where
:math:`N\min(\alpha, 1-\alpha)` is small compared with :math:`N`.  At
:math:`\alpha = 0.5` it retains about half of the sample, and the block
estimator is preferable.

Only the empirical-inverse estimator is supported.  The interpolated
estimator is rejected because its value is not an order statistic and
the exact binomial interval would no longer apply.

The sample size must be large enough for :class:`~openturns.QuantileConfidence`
to find exact ranks at the requested levels.  Otherwise the algorithm
raises an exception and suggests increasing the sample size or
decreasing the confidence level.

The following :class:`~openturns.ResourceMap` keys are used:

- ``ExactTailQuantileSimulation-MinimumChunkSize`` (``UnsignedInteger``, default: ``4096``): minimum number of observations drawn and processed per chunk, where the effective chunk size is the maximum of this value and the evaluation block size.
- ``QuantileSimulation-DefaultAlpha`` (``Scalar``, default: ``0.95``): default quantile level.
- ``QuantileSimulation-DefaultConfidenceLevel`` (``Scalar``, default: ``0.95``): default nominal confidence level.
- ``QuantileSimulation-DefaultSampleSize`` (``UnsignedInteger``, default: ``0``): default sample size, where ``0`` means that the sample size must be set before running.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.RandomVector(ot.Normal())
>>> algo = otexp.ExactTailQuantileSimulation(output, 0.999, 0.95)
>>> algo.setSampleSize(100000)
>>> algo.run()
>>> result = algo.getResult()
>>> estimate = result.getQuantileEstimate()
>>> retainedCount = result.getRetainedCount()
>>> tail = result.getTailDescription()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ExactTailQuantileSimulation::getExperiment
R"RAW(Accessor to the design of experiments.

Returns
-------
experiment : :class:`~openturns.WeightedExperiment`
    Independent design of experiments with uniform weights.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ExactTailQuantileSimulation::setExperiment
R"RAW(Accessor to the design of experiments.

Parameters
----------
experiment : :class:`~openturns.WeightedExperiment`
    Independent design of experiments with uniform weights.  Its
    distribution must match the input distribution of the output random
    vector.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ExactTailQuantileSimulation::hasExperiment
R"RAW(Accessor to the presence of a user-provided experiment.

Returns
-------
hasExperiment : bool
    Whether an experiment was provided.  When ``False``, a Monte Carlo
    experiment is built from the output distribution at run time.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ExactTailQuantileSimulation::getSampleSize
R"RAW(Accessor to the sample size.

Returns
-------
sampleSize : int
    Total number of model evaluations.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ExactTailQuantileSimulation::setSampleSize
R"RAW(Accessor to the sample size.

Parameters
----------
sampleSize : int
    Total number of model evaluations.  It must be positive and large
    enough for the exact binomial ranks to exist.
)RAW"
