%feature("docstring") OT::Gibbs
"Gibbs sampling method.

Refer to :ref:`bayesian_calibration`, :ref:`metropolis_hastings`.

Parameters
----------
samplers : sequence of :class:`~openturns.MetropolisHastings`
    List of samplers for each bloc of the chain

Notes
-----
Assume we want to sample from a complicated joint distribution.
The Gibbs algorithm is a Markov Chain Monte-Carlo algorithm
which calls samplers (implemented as :class:`~openturns.MetropolisHastings` objects)
in a predetermined sequence (by default)
or randomly: see :meth:`setUpdatingMethod` to manually choose the behavior,
and note the default behavior can be changed by setting the
`Gibbs-DefaultUpdatingMethod` :class:`~openturns.ResourceMap` key.
Each sampler samples from the conditional distributions of one or several components.

See Also
--------
RandomWalkMetropolisHastings, RandomVectorMetropolisHastings

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> chainDim = 3
>>> # Observations
>>> obsDim = 1
>>> obsSize = 10
>>> y = [-9.50794871493506, -3.83296694500105, -2.44545713047953,
...      0.0803625289211318, 1.01898069723583, 0.661725805623086,
...      -1.57581204592385, -2.95308465670895, -8.8878164296758,
...      -13.0812290405651]
>>> y_obs = ot.Sample([[yi] for yi in y])
>>> # Parameters
>>> covariates = ot.Sample(obsSize, chainDim)
>>> for i in range(obsSize):
...     for j in range(chainDim):
...         covariates[i, j] = (-2 + 5.0 * i / 9.0) ** j
>>> # Model
>>> fullModel = ot.SymbolicFunction(
...          ['p1', 'p2', 'p3', 'x1', 'x2', 'x3'],
...          ['p1*x1+p2*x2+p3*x3', '1.0'])
>>> parametersSet = range(chainDim)
>>> parametersValue = [0.0] * len(parametersSet) # 0.0 is a placeholder
>>> linkFunction = ot.ParametricFunction(fullModel, parametersSet, parametersValue)
>>> # Calibration parameters
>>> # Proposal distribution
>>> prop = ot.Uniform(-1.0, 1.0)
>>> # Prior distribution
>>> sigma0 = [10.0]*chainDim
>>> #  Covariance matrix
>>> Q0_inv = ot.CorrelationMatrix(chainDim)
>>> for i in range(chainDim): 
...     Q0_inv[i, i] = sigma0[i] * sigma0[i]
>>> mu0 = [0.0]*chainDim
>>> # x0 ~ N(mu0, sigma0)
>>> prior = ot.Normal(mu0, Q0_inv)
>>> # Conditional distribution y~N(z, 1.0)
>>> conditional = ot.Normal()
>>> # Create a Gibbs sampler
>>> coll = [ot.RandomWalkMetropolisHastings(prior, mu0, prop, [i]) for i in range(chainDim)]
>>> for mh in coll: mh.setLikelihood(conditional, y_obs, linkFunction, covariates)
>>> sampler = ot.Gibbs(coll)
>>> # Get a realization
>>> mu = sampler.getRealization()
>>> # Create a Gibbs sampler which updates one randomly chosen component at each step
>>> sampler2 = ot.Gibbs(coll)
>>> sampler2.setUpdatingMethod(ot.Gibbs.RANDOM_UPDATING)
>>> mu2 = sampler2.getRealization()"

// ---------------------------------------------------------------------

%feature("docstring") OT::Gibbs::getMetropolisHastingsCollection
"Get the samplers.

Returns
-------
samplers : list of :class:`~openturns.MetropolisHastings`
    List of samplers for each bloc of components"

// ---------------------------------------------------------------------

%feature("docstring") OT::Gibbs::getMetropolisHastingsCollection
"Get the list of Metropolis-Hastings samplers.

Returns
-------
samplers : sequence of :class:`MetropolisHastings`
    List of all Metropolis-Hastings samplers used in the Gibbs algorithm"

// ---------------------------------------------------------------------

%feature("docstring") OT::Gibbs::getUpdatingMethod
"Get how the order of the samplers is determined.

Available values are:

  * 0 (Gibbs.DETERMINISTIC_UPDATING): the samplers are sequentially called in the given order.

  * 1 (Gibbs.RANDOM_UPDATING): one of the samplers is randomly selected to produce the next realization.

Refer to the :meth:`setUpdatingMethod` documentation for details.

Returns
-------
updatingMethod : int
    See above for the possible values and their meaning."

// ---------------------------------------------------------------------

%feature("docstring") OT::Gibbs::setUpdatingMethod
"Set how the order of the samplers is determined.

Available values are:

  * 0 (Gibbs.DETERMINISTIC_UPDATING): the samplers are sequentially called in the given order.

  * 1 (Gibbs.RANDOM_UPDATING): one of the samplers is randomly selected to produce the next realization.

Note that under DETERMINISTIC_UPDATING,
all samplers are called in the specified order every time :meth:`getRealization` is called.
By contrast, under RANDOM_UPDATING, only one sampler is called
by :meth:`getRealization`, and this sampler is randomly chosen among all specified samplers.

Parameters
----------
updatingMethod : int
    See above for the possible values and their meaning."

// ---------------------------------------------------------------------

%feature("docstring") OT::Gibbs::getRecomputeLogPosterior
"Get the posterior recompute flags.

Returns
-------
recompute : :class:`~openturns.Indices`
    Whether the posterior at the state received from the previous sampler is recomputed.
    For debug use only."

// ---------------------------------------------------------------------

%feature("docstring") OT::Gibbs::getHistory
"Get the history storage.

Returns
-------
history : :class:`~openturns.HistoryStrategy`
    Used to record the chain."

// ---------------------------------------------------------------------

%feature("docstring") OT::Gibbs::setHistory
"Set the history storage.

Parameters
----------
history : :class:`~openturns.HistoryStrategy`
    Used to record the chain."
