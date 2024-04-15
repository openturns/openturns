.. _field_function:

Field functions
---------------

A field function :math:`f_{dyn}:\cD \times \Rset^{\inputDim} \rightarrow \cD' \times \Rset^q`
where :math:`\cD \in \Rset^n` and :math:`\cD' \in \Rset^p` is defined
by:

.. math::
  :label: dynFct

    \begin{aligned}
      f_{dyn}(\vect{t}, \vect{x}) = (t'(\vect{t}), v'(\vect{t}, \vect{x}))
    \end{aligned}

with :math:`t': \cD \rightarrow \cD'` and
:math:`v': \cD \times \Rset^{\inputDim} \rightarrow \Rset^q`.
A field function :math:`f_{dyn}` transforms a multivariate
stochastic process:

.. math::

    \begin{aligned}
      X: \Omega \times \cD \rightarrow \Rset^{\inputDim}\end{aligned}

where :math:`\cD \in \Rset^n` is discretized according to the
:math:`\cM` into the multivariate stochastic process:

.. math::

    \begin{aligned}
      Y=f_{dyn}(X)\end{aligned}

such that:

.. math::

    \begin{aligned}
      Y: \Omega \times \cD' \rightarrow \Rset^q\end{aligned}

where the mesh :math:`\cD' \in \Rset^p` is discretized according to
the :math:`\cM'`.

A field function :math:`f_{dyn}` also acts on fields and produces
fields of possibly different dimension (:math:`q\neq \inputDim`) and mesh
(:math:`\cD \neq \cD'` or :math:`\cM \neq \cM'`).



Value function
~~~~~~~~~~~~~~

A value function
:math:`f_{spat}: \cD \times \Rset^{\inputDim} \rightarrow \cD \times \Rset^q` is
a particular field function that leaves the mesh of a
field invariant and can be defined using a function
:math:`g : \Rset^{\inputDim}  \rightarrow \Rset^q` such that:

.. math::
    :label: spatFunc

      \begin{aligned}
       f_{spat}(\vect{t}, \vect{x})=(\vect{t}, g(\vect{x}))\end{aligned}

Let us note that the input dimension of :math:`f_{spat}` is still
:math:`d`, the dimension of :math:`\vect{x}`. Its output dimension is
equal to :math:`q`.
The creation of a value function requires the
function :math:`g` and the integer :math:`n`: the
dimension of the vertices of the mesh :math:`\cM`. These data are
required to test the compatibility of the dimensions when a composite
process is created using the value function.

Vertex value function
~~~~~~~~~~~~~~~~~~~~~

A vertex-value function
:math:`f_{temp}: \cD \times \Rset^{\inputDim} \rightarrow \cD \times \Rset^q` is
a particular field function that leaves the mesh of a
field invariant and is defined by a function
:math:`h :  \Rset^n \times \Rset^{\inputDim}  \rightarrow \Rset^q` such that:

.. math::
    :label: tempFunc

     \begin{aligned}
       f_{temp}(\vect{t}, \vect{x})=(\vect{t}, h(\vect{t},\vect{x}))\end{aligned}

Let us note that the input dimension of :math:`f_{temp}` is still
:math:`d`, the dimension of :math:`\vect{x}`. Its output dimension is
equal to :math:`q`.

.. topic:: API:

    - See :class:`~openturns.ValueFunction`
    - See :class:`~openturns.VertexValueFunction`

.. topic:: Examples:

    - See :doc:`/auto_functional_modeling/field_functions/plot_value_function`
    - See :doc:`/auto_functional_modeling/field_functions/plot_vertexvalue_function`
    - See :doc:`/auto_meta_modeling/fields_metamodels/plot_fieldfunction_metamodel`

