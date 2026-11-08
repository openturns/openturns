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
limiting case of subset simulation in which exactly one particle is
replaced at each level, so the level probability is not a tuning
parameter.  Each move selects the extreme particle, draws its input
from the distribution conditioned on exceeding the current extreme
output with a componentwise Metropolis-Hastings kernel and a burn-in of
:meth:`getBurnIn` transitions, and replaces the particle.  The point
estimate and the Poisson rank interval of Walter [walter2015]_ are
described in :any:`quantile_theory_rare_event`.  The finite-burn-in
kernel makes the achieved coverage asymptotic, so the result reports
``COVERAGE_ASYMPTOTIC``.

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
