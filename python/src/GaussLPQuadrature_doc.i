%feature("docstring") OT::GaussLPQuadrature
R"RAW(Gauss-LP quadrature rule builder.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Builds a quadrature rule integrating a finite set of scalar functions against
a measure. The builder is configured with the functions whose moments are
reproduced, the measure against which they are integrated and, optionally, a
sample of anchor nodes which must be retained in the rule. The quadrature
reproduces the moments of the first :math:`n` functions up to the requested
accuracy.

The functions do not need to be orthogonal, nor orthonormal, with respect to
the measure, and they do not need to be orthogonal with respect to any other
distribution either. The Gauss-LP algorithm only requires their moments
against the integration measure to be finite. Orthogonality is merely a
convenience: it makes the target moments equal to 0 or 1 and usually improves
the conditioning of the linear program. For a general collection, the moments
are first estimated with the measure-adapted quadrature rule described below,
and the linear program then finds nodes and positive weights reproducing
these moments. The functions should be linearly independent for the moment
constraints to be informative: a redundant function does not add information
and can make the linear program singular.

The anchor nodes are a candidate set which is used strictly: every retained
anchor has a positive weight in the returned rule. Duplicate anchors are
removed with a warning, and an anchor whose target probability density
function is not positive raises an exception. When the number of anchors is
at least the number of requested functions, the builder first tries to
reproduce the moments with the anchors alone: if it succeeds, no node is
added and the returned nodes are a subset of the anchors. Otherwise all
anchors are forced into the rule and the builder adds new nodes to complete
the moment constraints.

The algorithm proceeds by:

1. Computing the moments of the functions with a
   measure-adapted quadrature rule: an independent tensor Gauss product
   rule, then, when the standard space is standard normal, a Gauss-Hermite
   product rule transported through the inverse iso-probabilistic
   transformation, then a Gauss-Legendre rule on the numerical range with
   the density folded into the weights.
2. Drawing candidate points uniformly over the numerical range of the
   measure, independently of its mass, so that the candidates span the
   support rather than concentrate where the density is large. For an
   unbounded range, the infinite components are replaced by the extreme
   coordinates of the measure-adapted rule. The number of candidates is
   :math:`\max(n \alpha_S, n + \delta)`, where :math:`\alpha_S` is
   ``GaussLPQuadrature-AlphaS`` and :math:`\delta` is
   ``GaussLPQuadrature-Margin``.
3. Solving linear programs to find a sparse set of nodes with positive
   weights. When there are at least :math:`n` anchors, a minimum-weight
   program is first solved on the anchors alone; a feasible solution gives a
   rule without any added node. Otherwise, a first program maximizes the
   smallest anchor weight, then a minimum-weight program is solved with the
   anchor weights bounded from below by this positive optimum. When the
   sampled candidates cannot reproduce the moments, a measure-adapted
   tensor rule of roughly :math:`n` nodes is used as an additional candidate
   set; without anchors it becomes the starting rule directly.
4. Following the greedy clustering path, which merges the least weighted
   added node with its nearest neighbour until a target size is reached,
   from the smallest rule up to the largest starting rule. The latter has at
   most :math:`n` nodes unless the anchors already impose more than
   :math:`n` nodes. The anchors are kept by identity and are never clustered:
   only the added nodes take part in the condensation.
5. Local refinement via TNC optimization of each rule along the clustering
   path, skipped when the starting rule already meets the tolerance, with an
   explicit fallback to the clustered rule if the optimization fails. The
   first rule meeting the tolerance is returned, so that the number of nodes
   is the smallest one found on the path. The residual gradient is analytic.
   The anchor coordinates are fixed and the anchor weights stay positive
   during the refinement.

The number of nodes is therefore an output of the algorithm, not a
prescribed size. It is at most :math:`n` when there are fewer than
:math:`n` anchors. Use :meth:`buildRule` to obtain an immutable
:class:`~openturns.experimental.GaussLPQuadratureRule` that can be passed to
:class:`~openturns.ExperimentIntegration` or to any API expecting a weighted
experiment.

A dependent elliptical measure whose standard distribution is not the standard
normal distribution is rejected: the numerical range truncates its heavy
tails, so the Gauss-Legendre fallback cannot reproduce the moments.

.. note::
    The linear program requires HiGHS support. When OpenTURNS is built
    without HiGHS, :meth:`build` and :meth:`buildRule` raise
    *NotYetImplementedException*.

The Gauss-LP approach was introduced by [ryu2015]_ and extended to
the multivariate setting with a greedy clustering strategy by [jakeman2018]_.

Parameters
----------
functions : sequence of :class:`~openturns.Function`
    Scalar functions whose moments are reproduced. The quadrature integrates
    the first ``n`` functions exactly against *measure*. The functions do
    not need to be orthogonal with respect to *measure*.
measure : :class:`~openturns.Distribution`
    Integration measure.
anchors : 2-d sequence of float
    Optional anchor nodes used as a candidate pool. Duplicates are removed,
    every retained anchor has a positive weight in the rule and every anchor
    must have a positive target PDF.

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

.. list-table::
   :widths: 40 15 15 30
   :header-rows: 1

   * - Key
     - Type
     - Default value
     - Description
   * - ``GaussLPQuadrature-AlphaS``
     - ``UnsignedInteger``
     - ``100``
     - Multiplier for the number of LP candidate points.
   * - ``GaussLPQuadrature-Margin``
     - ``UnsignedInteger``
     - ``10``
     - Minimum number of candidate points beyond the number of functions. It is a heuristic ensuring that small values of ``GaussLPQuadrature-AlphaS`` still give the linear program more columns than moment constraints.
   * - ``GaussLPQuadrature-Ngauss``
     - ``UnsignedInteger``
     - ``100``
     - Requested order of the measure-adapted rule used for the moments.
   * - ``GaussLPQuadrature-MaximumTensorNodes``
     - ``UnsignedInteger``
     - ``1000000``
     - Maximum number of nodes of the tensor rule used for the moments. The marginal order is reduced when the full tensor product would exceed this budget, and the rule is rejected when the budget cannot even hold the order-2 tensor product.
   * - ``GaussLPQuadrature-Epsilon``
     - ``Scalar``
     - ``1.0e-5``
     - Tolerance on the norm of the quadrature residual; a warning is emitted above it.
   * - ``GaussLPQuadrature-ClusterPath``
     - ``Bool``
     - ``True``
     - Refine the rules obtained along the greedy clustering path from one node up to the largest rule, and stop at the first rule meeting ``GaussLPQuadrature-Epsilon``. When ``False``, only the largest rule is refined.
   * - ``GaussLPQuadrature-MaximumAbsoluteError``
     - ``Scalar``
     - ``1.0e-12``
     - Absolute error tolerance of the local refinement, superseding ``OptimizationAlgorithm-DefaultMaximumAbsoluteError``.
   * - ``GaussLPQuadrature-MaximumRelativeError``
     - ``Scalar``
     - ``1.0e-12``
     - Relative error tolerance of the local refinement, superseding ``OptimizationAlgorithm-DefaultMaximumRelativeError``.
   * - ``GaussLPQuadrature-MaximumObjectiveError``
     - ``Scalar``
     - ``1.0e-12``
     - Objective error tolerance of the local refinement, superseding ``OptimizationAlgorithm-DefaultMaximumResidualError``.
   * - ``GaussLPQuadrature-MaximumConstraintError``
     - ``Scalar``
     - ``1.0e-12``
     - Constraint error tolerance of the linear programming solve and of the local refinement, superseding ``OptimizationAlgorithm-DefaultMaximumConstraintError``.
   * - ``GaussLPQuadrature-Solver``
     - ``String``
     - ``choose``
     - HiGHS solver used for the linear programming solve.
   * - ``GaussLPQuadrature-FallbackSolver``
     - ``String``
     - ``ipm``
     - HiGHS solver used when the first linear programming solve fails; an empty value disables the retry.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> measure = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 2)
>>> functions = [ot.SymbolicFunction(['x', 'y'], [formula])
...              for formula in ['1.0', 'x', 'y', 'x * y']]
>>> quad = otexp.GaussLPQuadrature(functions, measure)
>>> nodes, weights = quad.build(3) # doctest: +SKIP
>>> rule = quad.buildRule(3) # doctest: +SKIP

A known three-point rule can be reused as anchors: when the anchors alone
reproduce the moments, the builder adds no node.

>>> measure1d = ot.Uniform(2.0, 4.0)
>>> functions1d = [ot.SymbolicFunction('x', formula)
...                for formula in ['1', 'x', 'x^2']]
>>> anchors = ot.Sample([[3.0 - (3.0 / 5.0) ** 0.5],
...                      [3.0],
...                      [3.0 + (3.0 / 5.0) ** 0.5]])
>>> anchored = otexp.GaussLPQuadrature(functions1d, measure1d, anchors)
>>> anchored.getAnchors().getSize()
3
>>> anchoredNodes, anchoredWeights = anchored.build(3) # doctest: +SKIP
)RAW"

%feature("docstring") OT::GaussLPQuadrature::build
"Build a quadrature rule.

Parameters
----------
n : int
    Number of functions to integrate exactly. It must be positive and
    must not exceed :meth:`getSize`.

Returns
-------
nodes : :class:`~openturns.Sample`
    The quadrature nodes. Their number is an output of the algorithm: it is
    at most ``n`` when there are fewer than ``n`` anchors. When a rule
    supported by the anchors alone reproduces the moments, no node is added
    and the nodes are a subset of the anchors. Otherwise all the anchors are
    the first nodes, in input order, followed by the added nodes. When
    ``GaussLPQuadrature-ClusterPath`` is ``True``, the smallest rule found
    along the greedy clustering path that meets the residual tolerance is
    returned; otherwise the full clustered support is refined.
weights : :class:`~openturns.Point`
    The quadrature weights, returned in the second output argument. They
    have one entry per node and are positive.
"

%feature("docstring") OT::GaussLPQuadrature::buildRule
"Build an immutable quadrature rule.

Parameters
----------
n : int
    Number of functions to integrate exactly. It must be positive and
    must not exceed :meth:`getSize`.

Returns
-------
rule : :class:`~openturns.experimental.GaussLPQuadratureRule`
    The immutable weighted experiment holding the nodes, the weights and
    the measure.
"

%feature("docstring") OT::GaussLPQuadrature::setAnchors
"Set the anchor nodes.

The anchors form a candidate pool used by :meth:`build` and
:meth:`buildRule`: every retained anchor has a positive weight in the rule.
When the anchors alone reproduce the moments, the rule is supported by a
subset of them and no node is added. Duplicate anchors are removed with a
warning, and every retained anchor must have a positive target PDF when the
measure is already set. Setting an empty sample removes all the anchors.

Parameters
----------
anchors : 2-d sequence of float
    Anchor nodes, dimensioned like the integration measure.
"

%feature("docstring") OT::GaussLPQuadrature::getAnchors
"Accessor to the anchor nodes.

Returns
-------
anchors : :class:`~openturns.Sample`
    The anchors retained by the builder, with duplicates removed.
"

%feature("docstring") OT::GaussLPQuadrature::setMeasureAndFunctions
"Accessor to the functions and measure used by the builder.

The current anchors are preserved and revalidated against the new measure:
an anchor outside the support of the new measure raises an exception and
leaves the builder unchanged.

Parameters
----------
measure : :class:`~openturns.Distribution`
    Integration measure.
functions : sequence of :class:`~openturns.Function`
    Non-empty collection of scalar functions dimensioned like *measure*. The
    functions do not need to be orthogonal with respect to *measure*.
"

%feature("docstring") OT::GaussLPQuadrature::getFunctionsCollection
"Accessor to the functions used by the builder.

Returns
-------
functions : :class:`~openturns.FunctionCollection`
    The scalar functions whose moments are reproduced.
"

%feature("docstring") OT::GaussLPQuadrature::getSize
"Accessor to the number of functions used by the builder.

Returns
-------
size : int
    Number of functions whose moments can be reproduced.
"

%feature("docstring") OT::GaussLPQuadrature::getMeasure
"Accessor to the integration measure.

Returns
-------
measure : :class:`~openturns.Distribution`
    Measure against which the moments are computed.
"
