%feature("docstring") OT::GaussLPQuadratureRule
"Immutable Gauss-LP quadrature rule.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class stores the nodes and weights produced by
:class:`~openturns.experimental.GaussLPQuadrature`. Unlike the builder, its
size is known when it is constructed: it is the number of nodes actually
returned by the algorithm, which may be smaller than the requested number of
functions. The stored distribution is the measure against which the rule was
built.

The rule is immutable: :meth:`setSize` rejects any size other than the stored
one and :meth:`setDistribution` always raises, so a built rule stays fixed.
:meth:`generateWithWeights` always returns the same nodes and weights, and
:meth:`isRandom` returns ``False``.

Parameters
----------
nodes : :class:`~openturns.Sample`
    The quadrature nodes.
weights : :class:`~openturns.Point`
    The quadrature weights, one per node.
measure : :class:`~openturns.Distribution`
    The measure against which the rule was built.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> measure = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 2)
>>> functions = [ot.SymbolicFunction(['x', 'y'], [formula])
...              for formula in ['1.0', 'x', 'y', 'x * y']]
>>> builder = otexp.GaussLPQuadrature(functions, measure)
>>> rule = builder.buildRule(3) # doctest: +SKIP
>>> nodes, weights = rule.generateWithWeights() # doctest: +SKIP
"

%feature("docstring") OT::GaussLPQuadratureRule::generateWithWeights
"Generate the stored quadrature rule.

Returns
-------
nodes : :class:`~openturns.Sample`
    The quadrature nodes.
weights : :class:`~openturns.Point`
    The quadrature weights, returned in the second output argument.
"

%feature("docstring") OT::GaussLPQuadratureRule::hasUniformWeights
"Tell whether the weights are uniform.

Returns
-------
uniform : bool
    ``True`` when all the weights are equal to ``1 / getSize()``.
"

%feature("docstring") OT::GaussLPQuadratureRule::isRandom
"Tell whether the rule is random.

Returns
-------
random : bool
    Always ``False``: the rule is built once and then stored.
"

%feature("docstring") OT::GaussLPQuadratureRule::setSize
"Set the number of nodes.

The size of a quadrature rule is fixed by its nodes. Setting the current
size is a no-op; setting another size raises an exception.

Parameters
----------
size : int
    The requested size.
"

%feature("docstring") OT::GaussLPQuadratureRule::setDistribution
"Set the measure.

The measure of a quadrature rule is fixed when the rule is built. Any call
raises an exception.

Parameters
----------
distribution : :class:`~openturns.Distribution`
    The requested measure.
"
