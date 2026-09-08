// SWIG file FastGaussQuadrature_doc.i
%feature("docstring") FastGaussQuadrature
R"RAW(Compute the polished Gauss quadrature nodes and weights of a Jacobi matrix.

.. warning::
    This feature is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Parameters
----------
gamma : sequence of float
    Diagonal of the Jacobi matrix, of length n.
b : sequence of float
    Off-diagonal of the Jacobi matrix, of length n; ``b[0]`` is unused.

Returns
-------
nodes : :class:`~openturns.Point`
    Nodes of the rule, strictly increasing.
weights : :class:`~openturns.Point`
    Weights of the rule, positive and summing to 1.)RAW"