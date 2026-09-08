// SWIG file FastJacobi_doc.i
%feature("docstring") FastJacobi
R"RAW(Compute the fast Gauss-Jacobi quadrature nodes and weights.

.. warning::
    This feature is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The measure is the normalized Jacobi weight
:math:`(1-x)^{\alpha}(1+x)^{\beta}` on :math:`[-1, 1]`.

Parameters
----------
n : int, :math:`0 < n`
    Number of quadrature nodes.
alpha : float, :math:`\alpha > -1`
    First shape parameter.
beta : float, :math:`\beta > -1`
    Second shape parameter.

Returns
-------
nodes : :class:`~openturns.Point`
    Nodes of the rule, strictly increasing.
weights : :class:`~openturns.Point`
    Weights of the rule, positive and summing to 1.)RAW"