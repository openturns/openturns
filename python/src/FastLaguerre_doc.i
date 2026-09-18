// SWIG file FastLaguerre_doc.i
%feature("docstring") FastLaguerre
R"RAW(Compute the fast Gauss-Laguerre quadrature nodes and weights.

.. warning::
    This feature is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The measure is the Gamma(k, 1, 0) distribution.

Parameters
----------
n : int, :math:`0 < n`
    Number of quadrature nodes.
k : float, :math:`k > 0`
    Shape parameter of the Gamma distribution.

Returns
-------
nodes : :class:`~openturns.Point`
    Nodes of the rule, strictly increasing.
weights : :class:`~openturns.Point`
    Weights of the rule, positive and summing to 1.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> nodes, weights = otexp.FastLaguerre(3, 1.0)
>>> print(nodes)
[0.415775,...,6.28995]
>>> print(weights)
[0.711093,...,0.0103893])RAW"