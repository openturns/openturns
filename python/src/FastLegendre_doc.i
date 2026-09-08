// SWIG file FastLegendre_doc.i
%feature("docstring") FastLegendre
R"RAW(Compute the fast Gauss-Legendre quadrature nodes and weights.

.. warning::
    This feature is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The measure is the uniform distribution on :math:`[-1, 1]`.

Parameters
----------
n : int, :math:`0 < n`
    Number of quadrature nodes.

Returns
-------
nodes : :class:`~openturns.Point`
    Nodes of the rule, in strictly increasing order.
weights : :class:`~openturns.Point`
    Weights of the rule, positive and summing to 2.)RAW"