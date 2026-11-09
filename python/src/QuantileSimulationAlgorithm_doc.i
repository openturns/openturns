%feature("docstring") OT::QuantileSimulationAlgorithm
R"RAW(Quantile simulation driven by a weighted experiment.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    QuantileSimulationAlgorithm(*output, alpha=0.95, beta=0.95*)

    QuantileSimulationAlgorithm(*output, experiment, alpha=0.95, beta=0.95*)

The algorithm estimates the quantile of a scalar output random vector
from a design produced by a :class:`~openturns.WeightedExperiment`.
The weights enter the weighted empirical cumulative distribution
function, not the output values: the algorithm never multiplies the
outputs by the weights.

For a sample :math:`\{(Y_i, w_i)\}_{i=1}^N` with finite non-negative
weights and a positive sum, the default estimate is the weighted
empirical inverse of the weighted empirical CDF built from the
normalized weights.  With uniform weights it reduces to the usual
empirical order statistic :math:`Y_{(\lceil N\alpha\rceil)}`.  The
weighted empirical CDF, the weighted inverse, the effective sample size
and the reference intervals are defined in
:any:`quantile_theory_weighted`.

The confidence interval and the coverage label depend on the
experiment:

- i.i.d. sampling with uniform weights and the empirical-inverse
  estimator: exact binomial order-statistic interval, coverage
  ``COVERAGE_EXACT``;
- randomized uniform-weight design that is not i.i.d. (for example a
  randomized low-discrepancy sequence or a Latin hypercube design):
  independent replicates and a Student :math:`t` interval on the
  replicate estimates, coverage ``COVERAGE_ASYMPTOTIC``;
- all other designs, including i.i.d. sampling with the interpolated
  estimator, weighted importance sampling and deterministic designs:
  weighted Woodruff interval on the single design.  The variance
  estimator assumes independent draws, so the coverage is
  ``COVERAGE_ASYMPTOTIC`` for i.i.d. draws from the proposal, as in
  :class:`~openturns.ImportanceSamplingExperiment`, and
  ``COVERAGE_NONE`` for a dependent randomized design or a
  deterministic design.

The replicate count only affects the randomized uniform-weight
non-i.i.d. branch.  It must be at least 2 and defaults to the value of
the :class:`~openturns.ResourceMap` key
``QuantileSimulationAlgorithm-DefaultReplicateCount`` (30).

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
experiment : :class:`~openturns.WeightedExperiment`
    Design of experiments.  Its distribution must match the input
    distribution of ``output``.  When omitted, a
    :class:`~openturns.MonteCarloExperiment` is built from the
    distribution of ``output``.
alpha : float
    Quantile level, in :math:`(0, 1)`.  Default is 0.95.
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.
    Default is 0.95.

See also
--------
QuantileSimulation, QuantileSimulationImplementation, QuantileSimulationResult,
openturns.WeightedExperiment, openturns.MonteCarloExperiment,
openturns.QuantileConfidence

Notes
-----
The sample size is the size of the experiment, and one run performs
``sampleSize`` model evaluations per experiment draw.  For the
replicate branch, the total number of evaluations is
``replicateCount * sampleSize``.

The replicate interval requires independent draws.  A
:class:`~openturns.LHSExperiment` reuses its stratum permutation from
one draw to the next when ``alwaysShuffle`` is false; the replicate
estimates are then correlated (and identical when ``randomShift`` is
false too), so such an experiment is rejected instead of reporting a
coverage that the design cannot support.

The weights are self-normalized, so an experiment whose weights sum to
one and an experiment whose weights sum to another positive constant
produce the same estimate.  The effective sample size reported in the
result is the Kish effective sample size :math:`\ESS` of the normalized
weights, defined in :any:`quantile_theory_definitions`.

The output must be scalar.  For a multivariate output, select a
marginal with ``output.getMarginal(i)`` or compose a scalar function
with :class:`~openturns.CompositeRandomVector`.

The weighted Woodruff interval is asymptotic and relies on a ratio
estimator central limit theorem; it is not a distribution-free
interval.  A deterministic design carries no sampling randomness, so
the algorithm reports ``COVERAGE_NONE`` for it: the interval describes
the design, not a repeated-sampling guarantee.

The algorithm uses the process-wide random generator of the
experiments; it does not expose a per-algorithm random stream.  Setting
the global seed before a run makes the run reproducible, but two
algorithms run concurrently are not stream-isolated.

When ``keepSample`` is enabled, the algorithm retains the input sample,
the output sample and the weight vector of the last run.  This costs
memory proportional to the sample size and is disabled by default.
For the replicate branch, only the first replicate is retained.

The convergence controls (``maximumOuterSampling``,
``maximumCoefficientOfVariation``, ``maximumStandardDeviation``,
``maximumTimeDuration`` and ``convergenceStrategy``), the evaluation
block size, and the progress and stop callbacks do not drive the number
of evaluations: the sample size is fixed by the experiment, the
replicate count and :meth:`setSampleSize`.  The block size is reported
in the result.

The following :class:`~openturns.ResourceMap` key is used:

- ``QuantileSimulationAlgorithm-DefaultReplicateCount``
  (``UnsignedInteger``, default: ``30``): default number of independent
  replicates of the randomized uniform-weight branch.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> output = ot.RandomVector(ot.Normal())
>>> algo = otexp.QuantileSimulationAlgorithm(output, 0.95, 0.95)
>>> algo.setSampleSize(10000)
>>> algo.run()
>>> result = algo.getResult()
>>> coverage = result.getCoverageType()
>>> interval = result.getConfidenceInterval()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationAlgorithm::getExperiment
R"RAW(Accessor to the design of experiments.

Returns
-------
experiment : :class:`~openturns.WeightedExperiment`
    Design of experiments used by the algorithm.  When no experiment
    was provided, the returned object is the default-constructed
    experiment and :meth:`hasExperiment` returns ``False``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationAlgorithm::setExperiment
R"RAW(Accessor to the design of experiments.

Parameters
----------
experiment : :class:`~openturns.WeightedExperiment`
    Design of experiments.  Its distribution must match the input
    distribution of the output random vector.

Notes
-----
Setting the experiment also sets the sample size to the size of the
experiment.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationAlgorithm::hasExperiment
R"RAW(Accessor to the experiment availability.

Returns
-------
hasExperiment : bool
    Whether an experiment was explicitly provided.  When ``False``,
    the algorithm builds a :class:`~openturns.MonteCarloExperiment`
    from the distribution of the output random vector at run time.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationAlgorithm::setSampleSize
R"RAW(Accessor to the sample size.

Parameters
----------
sampleSize : int
    Number of model evaluations performed by one experiment draw.
    It must be positive.

Notes
-----
When an experiment was provided, its size is updated as well.  The
maximum outer sampling is updated to
``ceil(sampleSize / blockSize)``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationAlgorithm::getReplicateCount
R"RAW(Accessor to the replicate count.

Returns
-------
replicateCount : int
    Number of independent experiment draws used by the replicate
    Student :math:`t` interval.  Default is 30.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationAlgorithm::setReplicateCount
R"RAW(Accessor to the replicate count.

Parameters
----------
replicateCount : int
    Number of independent experiment draws used by the replicate
    Student :math:`t` interval.  It must be at least 2.

Notes
-----
The replicate count is only used for a randomized design with uniform
weights that is not i.i.d., for example a randomized low-discrepancy
sequence or a Latin hypercube design.  Other designs use a single draw
and ignore it.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationAlgorithm::getKeepSample
R"RAW(Accessor to the keep-sample flag.

Returns
-------
keepSample : bool
    Whether the algorithm retains the input sample, the output sample
    and the weight vector of the last run.  Default is ``False``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationAlgorithm::setKeepSample
R"RAW(Accessor to the keep-sample flag.

Parameters
----------
keepSample : bool
    Whether the algorithm retains the input sample, the output sample
    and the weight vector of the last run.

Notes
-----
Retaining the samples costs memory proportional to the sample size.
For the replicate branch, only the first replicate is retained.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationAlgorithm::getInputSample
R"RAW(Accessor to the retained input sample.

Returns
-------
inputSample : :class:`~openturns.Sample`
    Input sample of the last run.  It is empty unless
    :meth:`getKeepSample` returns ``True``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationAlgorithm::getOutputSample
R"RAW(Accessor to the retained output sample.

Returns
-------
outputSample : :class:`~openturns.Sample`
    Output sample of the last run.  It is empty unless
    :meth:`getKeepSample` returns ``True``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuantileSimulationAlgorithm::getWeights
R"RAW(Accessor to the retained weights.

Returns
-------
weights : :class:`~openturns.Point`
    Weights of the last run, in the order of the retained input and
    output samples.  It is empty unless :meth:`getKeepSample` returns
    ``True``.
)RAW"
