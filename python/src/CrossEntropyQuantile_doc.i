%feature("docstring") OT::CrossEntropyQuantile
R"RAW(Cross-entropy quantile estimator.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    CrossEntropyQuantile(*output, alpha=0.95, beta=0.95*)

The algorithm estimates the quantile of a scalar output
:math:`Y=g(\mathbf X)` by importance sampling with a cross-entropy
auxiliary distribution [rubinstein2017]_.  A pilot sample of size
:meth:`getPilotSize` first locates the target tail: let
:math:`t_0` be its :math:`\alpha`-quantile.  The event
:math:`E=\{Y>t_0\}` (respectively :math:`E=\{Y<t_0\}` for the lower
tail) is then passed to a standard-space cross-entropy run with
intermediate quantile level :math:`\rho=` :meth:`getIntermediateQuantileLevel`
[deboer2005]_.  The run produces an auxiliary distribution
:math:`q_{\mathrm{CE}}=\mathcal N(\boldsymbol\mu,\boldsymbol\sigma)`
on the standard space.

The cross-entropy distribution concentrates on the target tail, so its
standard deviations are typically smaller than one.  With
:math:`q=q_{\mathrm{CE}}`, the importance weights
:math:`\varphi/q` have an infinite second moment, and their
self-normalized average is dominated by a few draws.  The estimator
therefore uses the defensive proposal

.. math::

    q = \mathcal N(\boldsymbol\mu,\mathbf s),
    \qquad
    s_i = \max(\sigma_i, s_{\min}),
    \qquad
    s_{\min} \ge 1,

which keeps the cross-entropy location and inflates every scale to at
least one.  When :math:`s_i\ge 1`, the second moment
:math:`\mathbb E_q[(\varphi/q)^2]` is finite, and a floor strictly
larger than one gives a safety margin.  The floor is controlled by
:meth:`setMinimumProposalStandardDeviation`.  A final sample
:math:`\mathbf U_1,\dots,\mathbf U_N\sim q` is drawn from this
proposal, with outputs :math:`Y_i=g(T^{-1}(\mathbf U_i))`, where
:math:`T^{-1}` is the inverse isoprobabilistic transformation of the
input distribution.

The quantile is estimated by self-normalized importance sampling.  With
the standard normal density :math:`\varphi` of the input dimension, the
weights are

.. math::

    w_i = \frac{\varphi(\mathbf U_i)}{q(\mathbf U_i)},
    \qquad
    \widehat F_N(y) =
    \frac{\sum_{i=1}^N w_i\,\mathbf 1\{Y_i\le y\}}{\sum_{i=1}^N w_i}.

The estimate is the weighted empirical inverse

.. math::

    \hat q_\alpha = Y_{(K)}, \qquad
    K = \inf\Bigl\{k : \sum_{i=1}^k W_{(i)} \ge \alpha\Bigr\},

where the outputs are sorted together with their normalized weights
:math:`W_{(i)}=w_{(i)}/\sum_j w_j`.  The weights are evaluated in the
log domain with a maximum-factor stabilization to avoid underflow.

The confidence interval is the weighted Woodruff interval
[woodruff1952]_: the variance of :math:`\widehat F_N(\hat q_\alpha)`
is estimated by

.. math::

    \widehat{\mathrm{Var}}\bigl(\widehat F_N(\hat q_\alpha)\bigr)
    = \frac{\sum_{i=1}^N w_i^2
      \bigl(\mathbf 1\{Y_i\le \hat q_\alpha\}
      - \widehat F_N(\hat q_\alpha)\bigr)^2}
      {\bigl(\sum_{i=1}^N w_i\bigr)^2},

and the interval is obtained by inverting
:math:`\widehat F_N` at the levels
:math:`\alpha \pm z_{1-\beta/2}\sqrt{\widehat{\mathrm{Var}}}`.  The
reported standard error is half the interval width divided by
:math:`z_{1-\beta/2}`.  The interval is asymptotic, and its quality
degrades when the effective sample size
:math:`(\sum_i w_i)^2/\sum_i w_i^2` becomes small.

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

See also
--------
SubsetSamplingQuantile, LastParticleQuantile, QuantileSimulationResult

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
    target quantile level :math:`\alpha`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CrossEntropyQuantile::setIntermediateQuantileLevel
R"RAW(Accessor to the intermediate quantile level.

Parameters
----------
level : float
    Quantile level used to build the intermediate thresholds of the
    cross-entropy sequence, in :math:`(0, 1)`.  It is distinct from the
    target quantile level :math:`\alpha`.  Default is 0.25.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CrossEntropyQuantile::getPilotSize
R"RAW(Accessor to the pilot sample size.

Returns
-------
pilotSize : int
    Number of pilot draws used to locate the target tail before the
    cross-entropy run.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CrossEntropyQuantile::setPilotSize
R"RAW(Accessor to the pilot sample size.

Parameters
----------
pilotSize : int
    Number of pilot draws used to locate the target tail before the
    cross-entropy run.  It must be at least 2.  Default is 1000.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CrossEntropyQuantile::getMinimumProposalStandardDeviation
R"RAW(Accessor to the minimum proposal standard deviation.

Returns
-------
value : float
    Floor :math:`s_{\min}` applied to the standard deviations of the
    defensive proposal.  It is at least one.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CrossEntropyQuantile::setMinimumProposalStandardDeviation
R"RAW(Accessor to the minimum proposal standard deviation.

Parameters
----------
value : float
    Floor :math:`s_{\min}` applied to the standard deviations of the
    defensive proposal.  It must be finite and at least one.  Values
    strictly larger than one give a safety margin against very
    concentrated cross-entropy proposals.  Default is 1.2.)RAW"
