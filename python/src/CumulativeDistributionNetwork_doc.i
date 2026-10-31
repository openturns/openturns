%feature("docstring") OT::CumulativeDistributionNetwork
R"RAW(Graphical model defined by a product of cumulative distribution functions.

A cumulative distribution network (CDN) is a :math:`\inputDim`-dimensional distribution whose cumulative distribution function is given as a product of lower dimensional cumulative distribution functions:

.. math::

    F(\vect{x}) = \prod_{i=1}^n F_i(\vect{x}_{J_i})

where :math:`\vect{x}_{J_i} = (x_j, j\in J_i)` and :math:`J_i` is the clique of black nodes linked to the red node :math:`i` in the bipartite graph.
The dimension :math:`\inputDim` is the number of black nodes, i.e. the cardinal of the union of the cliques.

The probability density function (or probability mass function for integral distributions) is the mixed derivative (or mixed backward difference) of this product:

.. math::

    f(\vect{x}) = \frac{\partial^{\inputDim} F}{\partial x_1 \cdots \partial x_{\inputDim}}(\vect{x}).

It is therefore not the product of the factor densities: the derivative-sum-product rule of [huang2011]_ distributes each differentiation to exactly one factor containing the variable, and sums the resulting assignments.
For a set :math:`S` of differentiated variables, each assignment :math:`\sigma : S \to \{1, \dots, n\}` maps a variable :math:`j` to a factor :math:`\sigma(j)` such that :math:`j \in J_{\sigma(j)}`, and

.. math::

    \frac{\partial^{|S|} F}{\partial \vect{x}_S}(\vect{x}) = \sum_{\sigma} \prod_{i=1}^n \frac{\partial^{|S_i(\sigma)|} F_i}{\partial \vect{x}_{S_i(\sigma)}}(\vect{x}_{J_i}),

where :math:`S_i(\sigma) = \{j \in S \mid \sigma(j) = i\}`.

Parameters
----------
distributions : list of :class:`~openturns.Distribution`
    List of the :math:`n` distributions associated to the red nodes of the graph.
    The :math:`i` th distribution must have the dimension of the :math:`i` th clique.
graph : :class:`~openturns.BipartiteGraph`
    Bipartite graph with :math:`n` red nodes.
    The clique of the red node :math:`i` gives the global indices of the variables of the :math:`i` th distribution.
    Every black node must belong to at least one clique.

Notes
-----
The CDF is evaluated as a product of the factor CDFs in log domain.
Coordinates equal to :math:`+\infty` are integrated out exactly by using the corresponding factor marginal, and a coordinate equal to :math:`-\infty` gives a zero CDF.

The PDF, log-PDF, conditional CDF, conditional PDF and conditional quantile use the derivative-sum-product algorithm of [huang2011]_:

- the derivative assignments of forest components are summed by dynamic programming on the factor graph;
- cyclic components are enumerated explicitly;
- the sums are accumulated with a log-sum-exp to avoid underflow;
- Gaussian factors use the exact Schur complement of the covariance matrix;
- independent factors factorise into products of univariate densities and CDFs.

For integral distributions, probability masses are the mixed backward differences of the product of factor CDFs.
They are evaluated exactly by variable elimination on a binary factor graph in log domain with a signed log-sum-exp [koller2009]_.
Distributions mixing continuous and discrete factors support the CDF only; their PDF and conditional services raise a ``NotYetImplementedException`` unless the independent-copula shortcut applies.
Sampling such a network when a variable is shared by several factors also requires conditional quantiles and raises a ``NotYetImplementedException``; networks whose factors do not share variables are sampled factor by factor.

The range of the distribution is the intersection of the ranges of the factors projected on the black nodes.

The following :class:`~openturns.ResourceMap` key is used:

- ``CumulativeDistributionNetwork-MaximumTerms`` (``UnsignedInteger``, default: ``1000000``): maximum number of derivative assignments, free-variable table entries or binary table entries enumerated by the density and conditional algorithms before a ``NotYetImplementedException`` is raised.

The generic CDF of a dependent non-Gaussian factor may underflow when the factor CDF itself underflows in linear scale, even though the surrounding product is accumulated in log domain.

Examples
--------
Create a network from three bivariate normal factors on a triangle:

>>> import openturns as ot
>>> graph = ot.BipartiteGraph([[0, 1], [1, 2], [2, 0]])
>>> distribution = ot.CumulativeDistributionNetwork([ot.Normal(2)] * 3, graph)
>>> distribution.getDimension()
3

Evaluate the CDF and the PDF:

>>> print('%.6f' % distribution.computeCDF([1.0, 2.0, -0.5]))
0.064354
>>> print('%.6f' % distribution.computePDF([0.5, -0.2, 0.3]))
0.075512

Evaluate a conditional distribution and an exact marginal:

>>> distribution = ot.CumulativeDistributionNetwork(
...     [ot.Normal(2)], ot.BipartiteGraph([[0, 1]]))
>>> print('%.6f' % distribution.computeConditionalCDF(-0.2, [0.5]))
0.420740
>>> print('%.6f' % distribution.computeConditionalPDF(-0.2, [0.5]))
0.391043
>>> distribution.getMarginal([0, 1]).getDimension()
2
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::getDistributionCollection
R"RAW(Get the distributions defining the CDN.

Returns
-------
distributions : list of :class:`~openturns.Distribution`
    The distributions associated to the red nodes, in graph order.
    Changing an element of the returned list does not modify the network.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::setDistributionCollection
R"RAW(Set the distributions defining the CDN.

Parameters
----------
distributions : list of :class:`~openturns.Distribution`
    The distributions associated to the red nodes, in graph order.
    The number of distributions and each dimension must match the current graph.

Notes
-----
The incidence caches and the range are rebuilt.
An ``InvalidArgumentException`` is raised, and the network is left unchanged, if the collection size or a distribution dimension is inconsistent with the graph, or if a distribution is invalid.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::getGraph
R"RAW(Get the bipartite graph defining the CDN.

Returns
-------
graph : :class:`~openturns.BipartiteGraph`
    The graph whose red node :math:`i` is linked to the variables of the :math:`i` th distribution.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::setGraph
R"RAW(Set the bipartite graph defining the CDN.

Parameters
----------
graph : :class:`~openturns.BipartiteGraph`
    The new graph.
    It must have as many red nodes as the distribution collection, and each red node must have as many black nodes as the dimension of the corresponding distribution.

Notes
-----
The incidence caches and the range are rebuilt.
An ``InvalidArgumentException`` is raised, and the network is left unchanged, if a red node has an empty clique, if a black node is repeated in a clique, or if the graph dimensions are inconsistent.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::getRealization
R"RAW(Get one realization of the CDN.

Returns
-------
realization : :class:`~openturns.Point`
    One realization of the distribution.

Notes
-----
When no variable is shared between factors the network is a product of independent blocks: each factor is sampled directly and its coordinates are dispatched.
Otherwise the realization is obtained by inversion of the joint CDF through the sequential conditional CDFs of the network, which is exact but may be expensive for dependent factors in high dimension.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::getSample
R"RAW(Get many realizations of the CDN.

Parameters
----------
size : int
    Number of realizations.

Returns
-------
sample : :class:`~openturns.Sample`
    The realizations, with the description of the distribution.

Notes
-----
The block-independent shortcut of :meth:`getRealization` is used when possible.
Otherwise the sample is obtained by the inherited inversion algorithm, one sequential conditional inversion per realization.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::computePDF
R"RAW(Compute the probability density or mass function.

Parameters
----------
x : sequence of float
    Point in :math:`\Rset^\inputDim`.

Returns
-------
f : float
    PDF value at *x* for a continuous network, PMF value at *x* for an integral network.

Notes
-----
For a continuous network the value is the mixed derivative of the product of the factor CDFs, computed with the derivative-sum-product algorithm in log domain and exponentiated.
For an integral network it is the mixed backward difference of the product of the factor CDFs, computed by variable elimination in log domain with a signed log-sum-exp.
A point with a non-integer coordinate in an integral network returns 0.

An ``InvalidArgumentException`` is raised if *x* does not have the dimension of the distribution.
A ``NotYetImplementedException`` is raised if the network mixes continuous and discrete factors, or if the number of enumerated terms exceeds ``CumulativeDistributionNetwork-MaximumTerms``.
If the value underflows, 0 is returned; use :meth:`computeLogPDF` for the tail.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::computeLogPDF
R"RAW(Compute the logarithm of the probability density or mass function.

Parameters
----------
x : sequence of float
    Point in :math:`\Rset^\inputDim`.

Returns
-------
logF : float
    Natural logarithm of the PDF at *x* for a continuous network, or of the PMF at *x* for an integral network.
    Returns :math:`-\infty` outside the support or when the probability is zero.

Notes
-----
The density is assembled in log domain with a log-sum-exp, so this method remains accurate when :meth:`computePDF` underflows.
The same restrictions on mixed continuous/discrete networks and on ``CumulativeDistributionNetwork-MaximumTerms`` apply.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::computeCDF
R"RAW(Compute the cumulative distribution function.

Parameters
----------
x : sequence of float
    Point in :math:`\Rset^\inputDim`.

Returns
-------
F : float
    CDF value at *x*, in :math:`[0, 1]`.

Notes
-----
The CDF is the product of the factor CDFs, accumulated in log domain.
Coordinates equal to :math:`+\infty` are integrated out exactly through the factor marginals; a coordinate equal to :math:`-\infty` gives zero.

The method is available for continuous, integral and mixed networks.
An ``InvalidArgumentException`` is raised if *x* does not have the dimension of the distribution.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::computeConditionalCDF
R"RAW(Compute the conditional CDF of a component given the previous ones.

Parameters
----------
xj : float
    Value of the conditioned component.
xcond : sequence of float
    Values of the conditioning components, i.e. the first components of the distribution.

Returns
-------
F : float
    Conditional CDF :math:`\Prob{X_{d_c} \leq x_j \mid X_0=xcond_0,\dots,X_{d_c-1}=xcond_{d_c-1}}`, where :math:`d_c` is the size of *xcond*.

Notes
-----
The conditioning variables are a prefix of the distribution and the conditioned variable is the next one, as required by the sequential conditional interface.

For an independent copula the result is the marginal CDF of component :math:`d_c`; an empty *xcond* gives the marginal CDF of the first component.
For continuous networks the conditional CDF is the ratio of mixed derivatives of the joint CDF, evaluated with the derivative-sum-product algorithm.
For integral networks the derivatives are mixed backward differences evaluated by variable elimination.
A mixed continuous/discrete network without independent copula raises a ``NotYetImplementedException``.

Infinite values are supported: :math:`x_j=+\infty` gives 1 and :math:`x_j=-\infty` gives 0.
An ``InvalidArgumentException`` is raised if *xcond* has dimension greater than or equal to the distribution dimension.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::computeConditionalPDF
R"RAW(Compute the conditional PDF of a component given the previous ones.

Parameters
----------
xj : float
    Value of the conditioned component.
xcond : sequence of float
    Values of the conditioning components, i.e. the first components of the distribution.

Returns
-------
f : float
    Conditional PDF (continuous network) or conditional PMF (integral network) at *xj* given *xcond*.

Notes
-----
The conditioning variables are a prefix of the distribution and the conditioned variable is the next one.

For an independent copula the result is the marginal PDF of component :math:`d_c`, where :math:`d_c` is the size of *xcond*.
For continuous networks the conditional density is the ratio of a mixed derivative of order :math:`d_c+1` to a mixed derivative of order :math:`d_c`.
For integral networks the corresponding mixed backward differences are evaluated exactly; a non-integer *xj* returns 0.
A mixed continuous/discrete network without independent copula raises a ``NotYetImplementedException``.

An ``InvalidArgumentException`` is raised if *xcond* has dimension greater than or equal to the distribution dimension.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::computeConditionalQuantile
R"RAW(Compute the conditional quantile of a component given the previous ones.

Parameters
----------
p : float
    Probability level in :math:`[0, 1]`.
xcond : sequence of float
    Values of the conditioning components, i.e. the first components of the distribution.

Returns
-------
q : float
    Conditional quantile :math:`\inf\{x \mid \Prob{X_{d_c} \leq x \mid X_0=xcond_0,\dots,X_{d_c-1}=xcond_{d_c-1}} \geq p\}`, where :math:`d_c` is the size of *xcond*.

Notes
-----
For an independent copula, an empty *xcond*, or a level equal to 0 or 1, the result is the marginal quantile of component :math:`d_c`.
For an integral network the smallest integer whose conditional CDF reaches *p* is returned by bracketing and integer bisection.
For a continuous network the quantile is obtained by bracketing and bisection of the conditional CDF.

An ``InvalidArgumentException`` is raised if *p* is outside :math:`[0, 1]` or if *xcond* has dimension greater than or equal to the distribution dimension.
A mixed continuous/discrete network without independent copula raises a ``NotYetImplementedException``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::getMarginal
R"RAW(Accessor to the marginal distributions.

Parameters
----------
i : int or list of ints, :math:`0 \leq i < \inputDim`
    Component or components of the marginal distribution.

Returns
-------
distribution : :class:`~openturns.Distribution`
    The exact marginal distribution of the selected components.

Notes
-----
The marginal is obtained by replacing each factor with its marginal over the selected variables and discarding factors that do not contribute.
It is returned as a :class:`~openturns.CumulativeDistributionNetwork`, except when a single factor contributes, in which case that factor marginal is returned directly.
When all components are selected the result is a copy of the distribution.
The description of the selected components is preserved.

An ``InvalidArgumentException`` is raised if the index list is empty, contains a duplicate or is out of range.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::isElliptical
R"RAW(Check whether the distribution is elliptical.

Returns
-------
isElliptical : bool
    True if and only if the network has exactly one factor and this factor is elliptical.

Notes
-----
This is a deliberately conservative characterization: some networks with several elliptical factors are elliptical as well, but are reported as non-elliptical.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::isContinuous
R"RAW(Check whether the distribution is continuous.

Returns
-------
isContinuous : bool
    True if and only if all factors are continuous.

Notes
-----
A continuous network supports the PDF and the continuous conditional services.
A network mixing continuous and discrete factors still supports the CDF.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::isDiscrete
R"RAW(Check whether the distribution is discrete.

Returns
-------
isDiscrete : bool
    True if and only if all factors are discrete.

Notes
-----
A discrete network may have a non-integral support.
Only integral networks (see :meth:`isIntegral`) support the PMF and the discrete conditional services.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::isIntegral
R"RAW(Check whether the distribution has an integer support.

Returns
-------
isIntegral : bool
    True if and only if all factors have an integer support.

Notes
-----
An integral network supports the PMF, the log-PMF and the discrete conditional services, computed from mixed backward differences of the product of factor CDFs.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::hasIndependentCopula
R"RAW(Check whether the distribution has an independent copula.

Returns
-------
hasIndependentCopula : bool
    True if and only if every factor has an independent copula.

Notes
-----
In that case each factor CDF factorises into univariate CDFs, so the joint CDF also factorises (possibly with repeated variables) and the conditional services reduce to univariate marginals.
This flag also enables the block-independent sampling shortcut when no variable is shared between factors.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::getParameter
R"RAW(Get the parameters of the distribution.

Returns
-------
parameter : :class:`~openturns.Point`
    Concatenation of the parameters of the factors, in graph order.

Notes
-----
The parameter vector is the concatenation of the parameters of the distributions in the collection.
Its layout is described by :meth:`getParameterDescription`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::setParameter
R"RAW(Set the parameters of the distribution.

Parameters
----------
parameter : sequence of float
    Concatenation of the parameters of the factors, in graph order.

Notes
-----
Each factor receives its slice of the parameter vector.
The update is atomic: if a factor rejects its slice, no factor is modified and the network keeps its previous parameters.
An ``InvalidArgumentException`` is raised if the size of *parameter* differs from :meth:`getParameterDimension`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CumulativeDistributionNetwork::getParameterDescription
R"RAW(Get the description of the parameters of the distribution.

Returns
-------
description : list of str
    Description of the concatenated parameter vector.

Notes
-----
Each entry is the parameter name of the factor suffixed with ``_distribution_i``, where :math:`i` is the index of the factor in the graph.
)RAW"
