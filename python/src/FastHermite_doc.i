// SWIG file FastHermite_doc.i
%feature("docstring") FastHermite
R"RAW(Compute the fast Gauss-Hermite quadrature nodes and weights.

.. warning::
    This feature is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The measure is the standard normal distribution.

Parameters
----------
n : int, :math:`0 < n`
    Number of quadrature nodes.

Returns
-------
nodes : :class:`~openturns.Point`
    Nodes of the rule, strictly increasing.
weights : :class:`~openturns.Point`
    Weights of the rule, positive and summing to 1.)RAW"