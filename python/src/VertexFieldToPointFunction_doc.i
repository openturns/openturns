%feature("docstring") OT::VertexFieldToPointFunction
R"RAW(Restriction of a field function at a specific node.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Let :math:`\cM_N` be a mesh of size :math:`N`, :math:`I = \{ i_0, \dots, i_{N'-1} \} \in \llbracket 0, N-1 \rrbracket`
be a set of indices of
cardinal :math:`N' \leq N` and let :math:`d \geq 1` be an integer.

A VertexFieldToPointFunction :math:`f` is defined by:

.. math::

    f: \left| \begin{array}{rcl}
               \cF(\cM_N \times \Rset^d ) & \rightarrow & (\Rset^d)^{N'} \\
                F & \mapsto & F'
              \end{array} \right.

where :math:`F'` is a vector such that for all :math:`k \in \llbracket 0, N'-1 \rrbracket`, we have:

.. math::

    F'[dk:(k+1)d] = F(v_{i_k}) \in \Rset^d

where :math:`v_{i_k}` is the node of index :math:`i_k` of the mesh :math:`\cM_N`. In other words, we have:

.. math::

    F' = (F(v_{i_0}), \dots, F(v_{i_{N'-1}}))


Parameters
----------
inputMesh : :class:`~openturns.Mesh`
    Mesh
dimension : int
    Input, output dimension
indices : sequence of int
    Mesh indices

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> N = 10
>>> mesh = ot.RegularGrid(0, 1, N)
>>> f = otexp.VertexFieldToPointFunction(mesh, 1, [-1 % N])
)RAW"
