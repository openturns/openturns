%feature("docstring") OT::CrossEntropyQuantile
R"RAW(Cross-entropy quantile estimator.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    CrossEntropyQuantile(*output, alpha=0.95, beta=0.95*)

The algorithm estimates the quantile of a scalar output
:math:`Y=\model(\inputRV)` by importance sampling with a cross-entropy
auxiliary distribution [rubinstein2017]_.  A pilot sample of size
:meth:`getPilotSize` first locates the target tail; the resulting
threshold event is passed to a standard-space cross-entropy run with
intermediate quantile level :meth:`getIntermediateQuantileLevel`
[deboer2005]_.  The final sample is drawn from a defensive Gaussian
proposal that keeps the cross-entropy location and floors every
standard deviation at :meth:`getMinimumProposalStandardDeviation`, and
the quantile is the weighted empirical inverse of the self-normalized
importance weights.  The estimator, the defensive proposal and the
weighted Woodruff interval are described in
:any:`quantile_theory_rare_event`.  The interval is asymptotic, and its
quality degrades when the effective sample size becomes small.

The sample size is the number :math:`N` of draws from the defensive
proposal used by the final weighted estimate.  The result sample size
also counts the pilot sample and the cross-entropy adaptation
evaluations.  The intermediate quantile level is a tuning parameter of
the cross-entropy sequence and is distinct from the target quantile
level :math:`\alpha`.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
alpha : float
    Quantile level, in :math:`(0, 1)`.  Default is 0.95.
beta : float
    Nominal confidence level of the interval, in :math:`(0, 1)`.
    Default is 0.95.

See also
--------
SubsetSamplingQuantile, LastParticleQuantile, QuantileSimulationResult

Notes
-----
The pilot sample is used only to place the cross-entropy event.  A
pilot threshold that is too far in the tail makes the event rarer than
the target and wastes auxiliary draws; a pilot threshold that is too
close to the centre makes the event too frequent and reduces the
resolution near the target quantile.  The default pilot size is a
compromise that is adequate for smooth outputs.

The auxiliary distribution returned in the result is the defensive
proposal :math:`q`, whose mean is the cross-entropy mean and whose
standard deviations are floored at
:meth:`getMinimumProposalStandardDeviation`.  The result also stores
the threshold sequence of the cross-entropy run through
:meth:`QuantileSimulationResult.getThresholdSequence`.

The following :class:`~openturns.ResourceMap` keys are used:

- ``CrossEntropyImportanceSampling-DefaultQuantileLevel`` (``Scalar``, default: ``0.25``): default intermediate quantile level of the cross-entropy sequence.
- ``CrossEntropyQuantile-DefaultPilotSize`` (``UnsignedInteger``, default: ``1000``): default pilot sample size.
- ``CrossEntropyQuantile-MinimumProposalStandardDeviation`` (``Scalar``, default: ``1.2``): default floor :math:`s_{\min}` on the defensive proposal standard deviations.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.RandomVector(ot.Normal())
>>> algo = otexp.CrossEntropyQuantile(output, 0.95, 0.95)
>>> algo.setSampleSize(5000)
>>> algo.setIntermediateQuantileLevel(0.25)
>>> algo.run()
>>> result = algo.getResult()
>>> estimate = result.getQuantileEstimate()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CrossEntropyQuantile::getIntermediateQuantileLevel
R"RAW(Accessor to the intermediate quantile level.

Returns
-------
level : float
    Quantile level used to build the intermediate thresholds of the
    cross-entropy sequence, in :math:`(0, 1)`.  It is distinct from the
    target quantile level :math:`\alpha`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CrossEntropyQuantile::setIntermediateQuantileLevel
R"RAW(Accessor to the intermediate quantile level.

Parameters
----------
level : float
    Quantile level used to build the intermediate thresholds of the
    cross-entropy sequence, in :math:`(0, 1)`.  It is distinct from the
    target quantile level :math:`\alpha`.  Default is 0.25.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CrossEntropyQuantile::getPilotSize
R"RAW(Accessor to the pilot sample size.

Returns
-------
pilotSize : int
    Number of pilot draws used to locate the target tail before the
    cross-entropy run.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CrossEntropyQuantile::setPilotSize
R"RAW(Accessor to the pilot sample size.

Parameters
----------
pilotSize : int
    Number of pilot draws used to locate the target tail before the
    cross-entropy run.  It must be at least 2.  Default is 1000.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CrossEntropyQuantile::getMinimumProposalStandardDeviation
R"RAW(Accessor to the minimum proposal standard deviation.

Returns
-------
value : float
    Floor :math:`s_{\min}` applied to the standard deviations of the
    defensive proposal.  It is at least one.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CrossEntropyQuantile::setMinimumProposalStandardDeviation
R"RAW(Accessor to the minimum proposal standard deviation.

Parameters
----------
value : float
    Floor :math:`s_{\min}` applied to the standard deviations of the
    defensive proposal.  It must be finite and at least one.  Values
    strictly larger than one give a safety margin against very
    concentrated cross-entropy proposals.  Default is 1.2.
)RAW"
