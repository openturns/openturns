%feature("docstring") OT::CumulativeDistributionNetworkFactory
R"RAW(Factory of cumulative distribution networks estimated by maximum likelihood.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The graph of the network is assumed to be known and only the parameters of the factor distributions are estimated.
Each red node is associated with a parametric family, given by a template distribution: the estimated factor has the same type as its template and receives the corresponding parameters.

The parameter vector is the concatenation, in graph order, of the parameters :math:`\vect{\theta}_i` of the factors, as returned by :meth:`~openturns.CumulativeDistributionNetwork.getParameter`.
The estimate maximizes the exact log-likelihood of the network:

.. math::

    \hat{\vect{\theta}} = \arg\max_{\vect{\theta}} \frac{1}{n}\sum_{k=1}^n \log f(\vect{x}_k;\vect{\theta})

where :math:`f` is the network density, obtained from the mixed derivative of the product of the factor CDFs [huang2011]_, and not from the product of the factor densities.
The maximization is performed by an :class:`~openturns.OptimizationAlgorithm` (``TNC`` by default) using the ``MaximumLikelihoodFactory-*`` :class:`~openturns.ResourceMap` keys, see the Notes section.

Parameters
----------
graph : :class:`~openturns.BipartiteGraph`
    Bipartite graph defining the network.
    Its red nodes are the factors and the clique of a red node gives the global indices of its variables.
distributions : list of :class:`~openturns.Distribution`
    Template distribution of each factor, in graph order.
    The :math:`i` th distribution must have the dimension of the :math:`i` th clique.
    Its parameters are used as the starting point of the optimization.

Notes
-----
The joint distribution is estimated consistently, but the split of the density between the factors may be weakly identified: several parameter vectors can produce nearly the same joint law, especially when a variable belongs to many cliques.
This happens even for a tree, where the factor CDFs are not the clique marginals of the joint distribution.
In that case the estimated factor parameters must not be interpreted individually, and confidence intervals obtained from :meth:`buildEstimator` may be wide.

The parameters are optimized in their natural parametrization: no transformation is applied.
When the admissible set of a factor is not the full space (for example the standard deviation of a normal factor must be positive), it is recommended to provide both a relevant starting point and relevant bounds.

The following :class:`~openturns.ResourceMap` keys are used:

- ``MaximumLikelihoodFactory-DefaultOptimizationAlgorithm`` (``str``, default: ``"TNC"``): name of the default optimization algorithm.
- ``MaximumLikelihoodFactory-MaximumCallsNumber`` (``UnsignedInteger``, default: ``1000``): maximum number of calls to the objective function.
- ``MaximumLikelihoodFactory-MaximumAbsoluteError`` (``Scalar``, default: ``1.0e-10``): absolute convergence tolerance.
- ``MaximumLikelihoodFactory-MaximumRelativeError`` (``Scalar``, default: ``1.0e-10``): relative convergence tolerance.
- ``MaximumLikelihoodFactory-MaximumObjectiveError`` (``Scalar``, default: ``1.0e-10``): objective convergence tolerance.
- ``MaximumLikelihoodFactory-MaximumConstraintError`` (``Scalar``, default: ``1.0e-10``): constraint convergence tolerance.

See also
--------
openturns.CumulativeDistributionNetwork, openturns.MaximumLikelihoodFactory

Examples
--------
Estimate the three factors of a triangle from a sample:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> graph = ot.BipartiteGraph([[0, 1], [1, 2], [2, 0]])
>>> reference = ot.CumulativeDistributionNetwork([ot.Normal(2)] * 3, graph)
>>> sample = reference.getSample(200)
>>> factory = otexp.CumulativeDistributionNetworkFactory(graph, [ot.Normal(2)] * 3)
>>> estimated = factory.build(sample)
>>> estimated.getDimension()
3
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::build
R"RAW(Estimate the distribution.

**Available usages**:

    build()

    build(*sample*)

    build(*parameters*)

Parameters
----------
sample : 2-d sequence of float
    Sample of the network, of dimension the number of black nodes of the graph.
parameters : sequence of float
    Concatenation, in graph order, of the parameters of the factor distributions.

Returns
-------
distribution : :class:`~openturns.CumulativeDistributionNetwork`
    The network built with the estimated parameters.

    In the first usage, the network is built with the parameters of the template distributions.
    In the second usage, the parameters are estimated by maximum likelihood.
    In the third usage, the given parameters are used as is.

Notes
-----
The sample dimension must match the number of black nodes of the graph, and its size must be positive.
An ``InvalidArgumentException`` is raised if the graph and the template distributions are inconsistent, or if the parameter size does not match :meth:`getParameterDimension`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::buildEstimator
R"RAW(Estimate the distribution and its parameter distribution.

Parameters
----------
sample : 2-d sequence of float
    Sample of the network, of dimension the number of black nodes of the graph.

Returns
-------
result : :class:`~openturns.DistributionFactoryResult`
    The result contains the estimated distribution and the distribution of its parameter, estimated by bootstrap.

Notes
-----
The number of bootstrap resamples is given by :meth:`~openturns.DistributionFactory.getBootstrapSize`.
Because the factor parameters may be weakly identified on a cyclic graph, the bootstrap distribution of the parameter can be wide even when the joint distribution is well estimated.

See also
--------
build
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::getGraph
R"RAW(Get the bipartite graph defining the network.

Returns
-------
graph : :class:`~openturns.BipartiteGraph`
    The graph whose red node :math:`i` is linked to the variables of the :math:`i` th factor.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::setGraph
R"RAW(Set the bipartite graph defining the network.

Parameters
----------
graph : :class:`~openturns.BipartiteGraph`
    The new graph.
    It must have as many red nodes as the distribution collection, and each red node is linked to as many black nodes as the dimension of the corresponding template distribution.

Notes
-----
The consistency of the graph with the template distributions is checked when the distribution is built.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::getDistributionCollection
R"RAW(Get the template distributions of the factors.

Returns
-------
distributions : list of :class:`~openturns.Distribution`
    The template distributions, in graph order.
    Changing an element of the returned list does not modify the factory.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::setDistributionCollection
R"RAW(Set the template distributions of the factors.

Parameters
----------
distributions : list of :class:`~openturns.Distribution`
    The template distributions, in graph order.
    The :math:`i` th distribution must have the dimension of the :math:`i` th clique.

Notes
-----
The consistency of the template distributions with the graph is checked when the distribution is built.
The parameters of the templates are used as the starting point of the estimation.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::getParameterDimension
R"RAW(Get the number of parameters of the network.

Returns
-------
dimension : int
    Sum of the parameter dimensions of the template distributions, in graph order.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::getStartingPoint
R"RAW(Get the starting point of the optimization.

Returns
-------
startingPoint : :class:`~openturns.Point`
    The starting point, in the concatenated parametrization of the factors.
    An empty point means that the parameters of the template distributions are used.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::setStartingPoint
R"RAW(Set the starting point of the optimization.

Parameters
----------
startingPoint : sequence of float
    Starting point, in the concatenated parametrization of the factors.
    An empty point means that the parameters of the template distributions are used.

Notes
-----
An ``InvalidArgumentException`` is raised if the point size is neither zero nor :meth:`getParameterDimension`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::getOptimizationAlgorithm
R"RAW(Get the optimization algorithm.

Returns
-------
solver : :class:`~openturns.OptimizationAlgorithm`
    The optimization algorithm used to maximize the log-likelihood.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::setOptimizationAlgorithm
R"RAW(Set the optimization algorithm.

Parameters
----------
solver : :class:`~openturns.OptimizationAlgorithm`
    The optimization algorithm used to maximize the log-likelihood.

Notes
-----
The problem is a maximization problem, its starting point and bounds are set by the factory.
If the solver has a starting point of the expected dimension, it is used when no explicit starting point was set.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::getOptimizationBounds
R"RAW(Get the optimization bounds.

Returns
-------
bounds : :class:`~openturns.Interval`
    The bounds of the parameter space.
    An interval of dimension zero means that the optimization is unbounded.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::setOptimizationBounds
R"RAW(Set the optimization bounds.

Parameters
----------
bounds : :class:`~openturns.Interval`
    Bounds of the parameter space, in the concatenated parametrization of the factors.
    An interval of dimension zero means that the optimization is unbounded.

Notes
-----
An ``InvalidArgumentException`` is raised if the interval dimension is neither zero nor :meth:`getParameterDimension`.
)RAW"
