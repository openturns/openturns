%feature("docstring") OT::CumulativeDistributionNetworkFactory
R"RAW(Factory of cumulative distribution networks.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Two estimation modes are available:

- when the graph is given, only the parameters of the factor distributions are estimated;
- when bounds on the graph characteristics are given instead, the graph and the factor distributions are estimated from the sample.

In the fixed-graph mode, each red node is associated with a parametric family, given by a template distribution: the estimated factor has the same type as its template and receives the corresponding parameters.
The parameter vector is the concatenation, in graph order, of the parameters :math:`\vect{\theta}_i` of the factors, as returned by :meth:`~openturns.CumulativeDistributionNetwork.getParameter`.
The estimate maximizes the exact log-likelihood of the network:

.. math::

    \hat{\vect{\theta}} = \arg\max_{\vect{\theta}} \frac{1}{n}\sum_{k=1}^n \log f(\vect{x}_k;\vect{\theta})

where :math:`f` is the network density, obtained from the mixed derivative of the product of the factor CDFs [huang2011]_, and not from the product of the factor densities.
The maximization is performed by an :class:`~openturns.OptimizationAlgorithm` (``TNC`` by default) using the ``MaximumLikelihoodFactory-*`` :class:`~openturns.ResourceMap` keys, see the Notes section.

In the structure-learning mode, only block-independent networks are considered: the cliques of the factors form a partition of the variables, so the network CDF factorises over the blocks.
The factor distributions are selected among the continuous univariate and multivariate factories using :meth:`~openturns.FittingTest.BestModelBIC`, and the partition is grown greedily by merging blocks while the BIC of the network improves.
A multivariate block is modelled either by a multivariate factory or by univariate marginals and a copula estimated on the pseudo-observations.

Available constructors:
    CumulativeDistributionNetworkFactory(*graph, distributions*)

    CumulativeDistributionNetworkFactory(*minimumFactorNumber, maximumFactorNumber, maximumFactorDimension*)

Parameters
----------
graph : :class:`~openturns.BipartiteGraph`
    Bipartite graph defining the network.
    Its red nodes are the factors and the clique of a red node gives the global indices of its variables.
distributions : list of :class:`~openturns.Distribution`
    Template distribution of each factor, in graph order.
    The :math:`i` th distribution must have the dimension of the :math:`i` th clique.
    Its parameters are used as the starting point of the optimization.
minimumFactorNumber : int
    Minimum number of factors (red nodes) of the estimated graph.
    It must be positive and not greater than the sample dimension.
maximumFactorNumber : int
    Maximum number of factors (red nodes) of the estimated graph.
    It must be greater than or equal to *minimumFactorNumber*.
maximumFactorDimension : int
    Maximum number of variables in a factor, i.e. the maximum clique size.
    It must be positive.

Notes
-----
In the fixed-graph mode, the joint distribution is estimated consistently, but the split of the density between the factors may be weakly identified: several parameter vectors can produce nearly the same joint law, especially when a variable belongs to many cliques.
This happens even for a tree, where the factor CDFs are not the clique marginals of the joint distribution.
In that case the estimated factor parameters must not be interpreted individually, and confidence intervals obtained from :meth:`buildEstimator` may be wide.

The parameters are optimized in their natural parametrization: no transformation is applied.
When the admissible set of a factor is not the full space (for example the standard deviation of a normal factor must be positive), it is recommended to provide both a relevant starting point and relevant bounds.

The following :class:`~openturns.ResourceMap` keys are used:

- ``MaximumLikelihoodFactory-DefaultOptimizationAlgorithm`` (``String``, default: ``TNC``): name of the default optimization algorithm.
- ``MaximumLikelihoodFactory-MaximumCallsNumber`` (``UnsignedInteger``, default: ``1000``): maximum number of calls to the objective function.
- ``MaximumLikelihoodFactory-MaximumAbsoluteError`` (``Scalar``, default: ``1.0e-10``): absolute convergence tolerance.
- ``MaximumLikelihoodFactory-MaximumRelativeError`` (``Scalar``, default: ``1.0e-10``): relative convergence tolerance.
- ``MaximumLikelihoodFactory-MaximumObjectiveError`` (``Scalar``, default: ``1.0e-10``): objective convergence tolerance.
- ``MaximumLikelihoodFactory-MaximumConstraintError`` (``Scalar``, default: ``1.0e-10``): constraint convergence tolerance.

In the structure-learning mode, the BIC of a block-independent network is the sum of the BICs of its blocks, so the score is exact for a given partition and the search is greedy over the partitions.
The search does not consider overlapping factors: every variable belongs to exactly one factor.
The factor distributions are selected among the parametric factories returned by :meth:`~openturns.DistributionFactory.GetContinuousUniVariateFactories` and :meth:`~openturns.DistributionFactory.GetContinuousMultiVariateFactories`; the non-parametric histogram and Bernstein copula factories are excluded.
The BIC is also used to select the factor distributions, so the reported criterion is optimistic for the selected model, and the structure may be weakly identified when several partitions have close BIC values.
:meth:`buildEstimator` is not available in this mode because the dimension of the parameter depends on the estimated structure.
The search fits every candidate block, so its cost grows quickly with the sample dimension; it is intended for moderate dimensions.

See also
--------
openturns.CumulativeDistributionNetwork, openturns.MaximumLikelihoodFactory

Examples
--------
Estimate the two factors of a network with independent variables:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> graph = ot.BipartiteGraph([[0], [1]])
>>> reference = ot.CumulativeDistributionNetwork([ot.Normal(), ot.Normal()], graph)
>>> sample = reference.getSample(200)
>>> factory = otexp.CumulativeDistributionNetworkFactory(graph, [ot.Normal(), ot.Normal()])
>>> estimated = factory.build(sample)
>>> estimated.getDimension()
2

Estimate the graph and the factor distributions of a network from a sample:

>>> factory = otexp.CumulativeDistributionNetworkFactory(1, 3, 2)
>>> estimated = factory.build(sample)
>>> estimated.getDimension()
2
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
    Sample of the network.
    In the fixed-graph mode, its dimension must be the number of black nodes of the graph.
    In the structure-learning mode, its dimension is the number of variables of the network.
parameters : sequence of float
    Concatenation, in graph order, of the parameters of the factor distributions.

Returns
-------
distribution : :class:`~openturns.CumulativeDistributionNetwork`
    The network built with the estimated parameters.

    In the first usage, the network is built with the parameters of the template distributions.
    In the second usage, the parameters are estimated by maximum likelihood in the fixed-graph mode, or the graph and the factor distributions are estimated in the structure-learning mode.
    In the third usage, the given parameters are used as is.

Notes
-----
In the fixed-graph mode, the sample dimension must match the number of black nodes of the graph, and its size must be positive.
An ``InvalidArgumentException`` is raised if the graph and the template distributions are inconsistent, or if the parameter size does not match :meth:`getParameterDimension`.

In the structure-learning mode, only the second usage is available.
The sample size must be positive, the minimum number of factors must not exceed the sample dimension, and the bounds must be compatible with a partition of the variables.
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
The number of bootstrap resamples is given by :meth:`~openturns.experimental.CumulativeDistributionNetworkFactory.getBootstrapSize`.
Because the factor parameters may be weakly identified on a cyclic graph, the bootstrap distribution of the parameter can be wide even when the joint distribution is well estimated.

This method is not available in the structure-learning mode, because the dimension of the parameter depends on the estimated graph.

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

Notes
-----
This method is not available in the structure-learning mode: the graph is estimated by :meth:`build` and is available from the returned distribution.
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
This method is not available in the structure-learning mode.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::getDistributionCollection
R"RAW(Get the template distributions of the factors.

Returns
-------
distributions : list of :class:`~openturns.Distribution`
    The template distributions, in graph order.
    Changing an element of the returned list does not modify the factory.

Notes
-----
In the structure-learning mode, the templates are not used and an empty list is returned.
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
This method is not available in the structure-learning mode.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::isStructureLearning
R"RAW(Whether the graph is estimated from the sample.

Returns
-------
structureLearning : bool
    True if the factory has been built with the bounds constructor, false if the graph has been given explicitly.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::getMinimumFactorNumber
R"RAW(Get the minimum number of factors of the estimated graph.

Returns
-------
minimumFactorNumber : int
    Minimum number of red nodes of the estimated graph.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::getMaximumFactorNumber
R"RAW(Get the maximum number of factors of the estimated graph.

Returns
-------
maximumFactorNumber : int
    Maximum number of red nodes of the estimated graph.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::getMaximumFactorDimension
R"RAW(Get the maximum dimension of a factor of the estimated graph.

Returns
-------
maximumFactorDimension : int
    Maximum number of variables in a factor.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetworkFactory::getParameterDimension
R"RAW(Get the number of parameters of the network.

Returns
-------
dimension : int
    Sum of the parameter dimensions of the template distributions, in graph order.
    In the structure-learning mode, the graph is not known before the estimation and this method returns 0.
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
