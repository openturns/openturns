%feature("docstring") OT::VertexFieldToPointFunction
R"RAW(Restriction of a field function at some specific nodes.

Let :math:`\cM_N` be a mesh of size :math:`N`, :math:`I = \{ i_0, \dots, i_{N'-1} \} \in \llbracket 0, N-1 \rrbracket`
be a set of indices of
cardinal :math:`N' \leq N` and let :math:`d \geq 1` be an integer.

A VertexFieldToPointFunction :math:`f` is defined by:

.. math::

    f: \left| \begin{array}{rcl}
               \cF(\cM_N, \Rset^d ) & \rightarrow & (\Rset^d)^{N'} \\
                g & \mapsto & \vect{F}
              \end{array} \right.

where :math:`\vect{F}` is a vector such that for all :math:`k \in \llbracket 0, N'-1 \rrbracket`, we have:

.. math::

    \vect{F}[dk:(k+1)d] = g(v_{i_k}) \in \Rset^d

where :math:`v_{i_k}` is the node of index :math:`i_k` of the mesh :math:`\cM_N`. In other words, we have:

.. math::

    \vect{F} = (g(v_{i_0}), \dots, g(v_{i_{N'-1}}))


Parameters
----------
inputMesh : :class:`~openturns.Mesh`
    Mesh, :math:`\cM_N`
dimension : int
    Input, output dimension, :math:`d \geq 1`
indices : sequence of int
    Mesh indices, :math:`I`

Examples
--------
>>> import openturns as ot
>>> N = 10
>>> mesh = ot.RegularGrid(0, 1, N)
>>> f = ot.VertexFieldToPointFunction(mesh, 1, [-1 % N])
)RAW"
