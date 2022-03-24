%feature("docstring") OT::RandomVectorMetropolisHastings
"Simple Metropolis-Hastings sampler defined from a random variable.

Refer to :ref:`bayesian_calibration`, :ref:`metropolis_hastings`.

Parameters
----------
randomVector : :class:`~openturns.RandomVector`
    The random variable from which to update the chain
initialState : sequence of float
    Initial state of the chain
marginalIndices : sequence of int, optional
    Indices of the components to be updated.
    If not specified, all components are updated.
    The number of updated components must be equal to the dimension of *proposal*.
linkFunction : :class:`~openturns.Function`, optional
    Link between the state of the chain and the parameters of *randomVector*.
    If not provided, then the parameters of *randomVector* are not updated,
    which means that samples from *randomVector* are produced independently
    from the state of the Markov chain.

Notes
-----
This class creates a Markov chain from a :class:`~openturns.RandomVector`.
It updates the parameters of the random vector
based on the current state of the Markov chain,
gets a realization from the random vector with the updated parameters,
and then uses it to update the Markov chain.

If no likelihood is set with the :meth:`setLikelihood` method,
then it can be viewed as trivial Metropolis-Hastings algorithm
where the proposal distribution is equal to the target distribution,
so the Metropolis-Hastings ratio is always equal to 1
and the candidate is always accepted.

If a likelihood is set, then the Metropolis-Hastings ratio
is the ratio of the likelihoods of the new and of the current state.

See Also
--------
Gibbs, RandomWalkMetropolisHastings

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)

Build a random vector and choose the initial state of the Markov chain:

>>> randomVector = ot.RandomVector(ot.Normal())
>>> initialState = [3.0]

We can also link the parameters of the random vector to the state of the chain,
Let us link the parameters of the random vector to the state of the chain.
Here the parameters of the random vector are the parameters of its distribution:

>>> linkFunction = ot.SymbolicFunction(['x'], ['x', '0.1'])

The link function makes the first parameter of the normal distribution (the mean)
equal to the current value of the Markov chain.
Its standard deviation remains constant: :math:`0.1`.
That way we construct a random walk with normal steps of standard deviation :math:`0.1`.

>>> sampler = ot.RandomVectorMetropolisHastings(randomVector, initialState, [0], linkFunction)
>>> x = sampler.getSample(10)"

// ---------------------------------------------------------------------

%feature("docstring") OT::RandomVectorMetropolisHastings::getRandomVector
"Random vector accessor.

Returns
-------
randomVector : :class:`~openturns.RandomVector`
    The random variable from which to update the chain"

// ---------------------------------------------------------------------

%feature("docstring") OT::RandomVectorMetropolisHastings::setRandomVector
"Random vector accessor.

Parameters
----------
randomVector : :class:`~openturns.RandomVector`
    The random variable from which to update the chain"  
