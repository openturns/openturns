%feature("docstring") OT::MixedHistogramFiniteDiscreteDistribution
"Mixed Histogram/FiniteDiscreteDistribution distribution.

Defines a distribution with Histogram (continuous) or FiniteDiscreteDistribution (discrete) marginals,
it is meant to be used as a specialization of the Mixture for efficient sampling.

Each marginal is defined from a list of float that either define the intervals on which
the marginal is uniform (equivalent to a :class:`~openturns.Histogram` distribution),
or the set of points that define the support of the discrete marginal
(equivalent to an :class:`~openturns.FiniteDiscreteDistribution` distribution).

Weights are required for each combination of intervals or discrete points,
ie for a dimension 2, with an Histogram component with 3 intervals (defined from 4 ticks) and
a discrete component with 3 values (3 ticks), 9 probability values are required, see example below.

Parameters
----------
ticksCollection : sequence of sequence of float
    List of ticks for each component
kind : sequence of int
    Either MixedHistogramFiniteDiscreteDistribution.DISCRETE or MixedHistogramFiniteDiscreteDistribution.CONTINUOUS
probabilityTable : sequence of float
    List of probabilities

See also
--------
Mixture, Histogram, FiniteDiscreteDistribution

Examples
--------
>>> import openturns as ot
>>> ticksCollection = [[0.0, 1.0, 3.0, 6.0], [1.0, 2.0, 3.0]]
>>> kind = [ot.MixedHistogramFiniteDiscreteDistribution.CONTINUOUS, ot.MixedHistogramFiniteDiscreteDistribution.DISCRETE]
>>> probabilityTable = [0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45]
>>> distribution = ot.MixedHistogramFiniteDiscreteDistribution(ticksCollection, kind, probabilityTable)
>>> distribution.setDescription(['A', 'B'])
>>> sample = distribution.getSample(5)"

// ----------------------------------------------------------------------------

%feature("docstring") OT::MixedHistogramFiniteDiscreteDistribution::asMixture
"Conversion as a Mixture.

Returns
-------
mixture : :class:`~openturns.Mixture`
    Mixture distribution"

// ----------------------------------------------------------------------------

%feature("docstring") OT::MixedHistogramFiniteDiscreteDistribution::setKind
"Kind accessor.

Parameters
----------
kind : sequence of int
    0=discrete, 1=continuous"

// ----------------------------------------------------------------------------

%feature("docstring") OT::MixedHistogramFiniteDiscreteDistribution::getKind
"Kind accessor.

Returns
-------
kind : sequence of int
    0=discrete, 1=continuous"

// ----------------------------------------------------------------------------

%feature("docstring") OT::MixedHistogramFiniteDiscreteDistribution::setProbabilityTable
"Probability table accessor.

Parameters
----------
probabilityTable : sequence of float
    List of probabilities"

// ----------------------------------------------------------------------------

%feature("docstring") OT::MixedHistogramFiniteDiscreteDistribution::getProbabilityTable
"Probability table accessor.

Returns
-------
probabilityTable : sequence of float
    List of probabilities"

// ----------------------------------------------------------------------------

%feature("docstring") OT::MixedHistogramFiniteDiscreteDistribution::setTicksCollection
"Ticks accessor.

Parameters
----------
ticksCollection : sequence of sequence of float
    ticksCollection"

// ----------------------------------------------------------------------------

%feature("docstring") OT::MixedHistogramFiniteDiscreteDistribution::getTicksCollection
"Ticks accessor.

Returns
-------
ticksCollection : sequence of sequence of float
    ticksCollection"
