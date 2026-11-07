%feature("docstring") OT::LastParticleQuantile
R"RAW(Last-particle quantile estimator.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    LastParticleQuantile(*output, alpha=0.95, beta=0.95*)

The last-particle algorithm of Guyader, Hengartner and Matzner-Løber
[guyader2011]_ estimates an extreme quantile with a population of
:math:`N` particles and one model evaluation per move.  It is the
limiting case of subset simulation in which the conditional probability
is :math:`p_0=1-1/N`: exactly one particle is replaced at each level,
the level probability is not a tuning parameter, and, in the idealized
process with independent conditional draws, the number of moves needed
to reach the target tail has an exact Poisson law.

Let :math:`p=1-\alpha` for the upper tail (:math:`p=\alpha` for the
lower tail).  After an initial i.i.d. population
:math:`(\mathbf X_i,Y_i)_{i=1}^N`, each move performs the following
steps:

1. select the extreme particle :math:`i_m`;
2. draw :math:`\mathbf X_{i_m}` from the input distribution conditioned
   on the event that it exceeds the current extreme output, using the
   componentwise Metropolis-Hastings kernel of the subset sampler with
   a burn-in of :meth:`getBurnIn` transitions;
3. replace the extreme particle by the new draw.

For the upper tail the extreme particle is the one with the smallest
output, so the conditional event is :math:`Y>Y_{\min}`; for the lower
tail it is the one with the largest output and the event is
:math:`Y<Y_{\max}`.  The thresholds recorded at each move are the
successive extreme values.  Their level probabilities are approximately
:math:`(1-1/N)^m`.

The number of moves needed to cross the true quantile is Poisson with
mean :math:`\lambda=-N\log p`.  The algorithm runs up to the upper
Poisson rank

.. math::

    U = \left\lceil Q_{\mathrm{Pois}(\lambda)}\!\left(\frac{1+\beta}{2}\right) \right\rceil,

and reports the reduced-bias midpoint estimate of Walter
[walter2015]_,

.. math::

    \hat q = \frac{t_{M-1}+t_M}{2},
    \qquad M = \lceil -N\log p\rceil,

where :math:`t_k` is the threshold of move :math:`k`.  The confidence
interval is the Poisson rank interval :math:`[t_L,t_U]` (reversed for
the lower tail), where :math:`L` and :math:`U` are the Poisson quantiles
at levels :math:`(1-\beta)/2` and :math:`(1+\beta)/2`.  Its coverage is
exact for the idealized process in which each move produces an
independent draw from the conditional distribution.  The implementation
uses a finite-burn-in Metropolis-Hastings kernel instead, so the
achieved coverage is asymptotic and the result reports
``COVERAGE_ASYMPTOTIC``.  The Poisson ranks are available through
:meth:`QuantileSimulationResult.getPoissonRankLower` and
:meth:`QuantileSimulationResult.getPoissonRankUpper`.

The particle count must be at least 10.  The sample size is an alias
for the particle count: :meth:`setSampleSize` and
:meth:`setParticleCount` are equivalent.

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
The algorithm is intended for continuous outputs.  With a discrete
output, ties can leave no particle strictly beyond the current extreme
value; the algorithm then raises ``NotDefinedException`` rather than
changing the conditional event.

The two tails are handled symmetrically by reversing the extreme
selection and the comparison operator.  The threshold sequence is
nondecreasing for the upper tail and nonincreasing for the lower tail.

The following :class:`~openturns.ResourceMap` keys are used:

- ``LastParticleQuantile-DefaultParticleCount`` (``UnsignedInteger``, default: ``1000``): default number of particles.
- ``LastParticleQuantile-DefaultBurnIn`` (``UnsignedInteger``, default: ``10``): default number of burn-in transitions.
- ``SubsetSampling-DefaultProposalRange`` (``Scalar``, default: ``2.0``): default random-walk proposal range.

See also
--------
SubsetSamplingQuantile, CrossEntropyQuantile, QuantileSimulationResult

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.RandomVector(ot.Normal())
>>> algo = otexp.LastParticleQuantile(output, 0.95, 0.95)
>>> algo.setParticleCount(500)
>>> algo.run()
>>> result = algo.getResult()
>>> estimate = result.getQuantileEstimate()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LastParticleQuantile::getParticleCount
R"RAW(Accessor to the particle count.

Returns
-------
particleCount : int
    Number of particles maintained by the last-particle process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LastParticleQuantile::setParticleCount
R"RAW(Accessor to the particle count.

Parameters
----------
particleCount : int
    Number of particles maintained by the last-particle process.  It
    must be at least 10.  Default is 1000.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LastParticleQuantile::getSampleSize
R"RAW(Accessor to the particle count.

Returns
-------
sampleSize : int
    Number of particles maintained by the last-particle process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LastParticleQuantile::setSampleSize
R"RAW(Accessor to the particle count.

Parameters
----------
sampleSize : int
    Number of particles maintained by the last-particle process.  It
    must be at least 10 and is equivalent to
    :meth:`setParticleCount`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LastParticleQuantile::getBurnIn
R"RAW(Accessor to the burn-in.

Returns
-------
burnIn : int
    Number of Metropolis-Hastings transitions performed before each
    replacement particle is retained.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LastParticleQuantile::setBurnIn
R"RAW(Accessor to the burn-in.

Parameters
----------
burnIn : int
    Number of Metropolis-Hastings transitions performed before each
    replacement particle is retained.  It must be positive.  Default is
    10.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LastParticleQuantile::getProposalRange
R"RAW(Accessor to the proposal range.

Returns
-------
proposalRange : float
    Range of the componentwise random-walk proposal used by the
    conditional sampler.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LastParticleQuantile::setProposalRange
R"RAW(Accessor to the proposal range.

Parameters
----------
proposalRange : float
    Range of the componentwise random-walk proposal used by the
    conditional sampler.  It must be finite and positive.  Default is
    2.0.)RAW"
