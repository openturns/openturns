.. _taylor_expansion:

Taylor expansion
----------------

Let :math:`h: \Rset^{\inputDim} \rightarrow \Rset^q` be a twice differentiable mapping.

First-order Taylor expansion
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first-order Taylor expansion of :math:`h` at the point :math:`\ux_0` is the function
:math:`\widetilde{h}: \Rset^{\inputDim} \rightarrow \Rset^q` defined for each marginal function
:math:`h_k` of :math:`h`  by the equation:

.. math::

   \widetilde{h}_k(\ux) = h_k(\ux_0) + \sum_{i=1}^{\inputDim} \left(\frac{\partial h_k}{\partial x_i}\right)(\ux_0)\left(x_i - x_{0,i} \right)

for :math:`k \in \{1, ..., q\}` which can be written as:

.. math::

    \widetilde{h}(\ux) = h(\ux_0) + \mat{L} (\ux-\ux_0)


where :math:`\mat{L} = (L_{ki})_{1 \leq k \leq \inputDim, 1\leq i \leq \inputDim}` is the Jacobian
matrix evaluated at the point :math:`\ux_0`:

.. math::

    L_{ki} = \left(\frac{\partial h_k}{\partial x_i}\right)(\ux_0)

for :math:`i \in \{1, ..., \inputDim\}` and :math:`j \in \{1, ..., \inputDim\}`.


Second-order Taylor expansion
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The second-order Taylor expansion of :math:`h` at the point :math:`\ux_0` is the function
:math:`\widetilde{h}: \Rset^{\inputDim} \rightarrow \Rset^q` defined for each marginal function
:math:`h_k` of :math:`h`  by the equation:

.. math::

   \widetilde{h}_k(\ux) = h_k(\ux_0) + \sum_{i=1}^{\inputDim}  \left(\frac{\partial h_k}{\partial x_i}\right)(\ux_0)
   \left(x_i - x_{0,i} \right) +  \frac{1}{2} \sum_{i,j = 1}^\inputDim \left( \frac{\partial^2 h_k}{\partial x_i
   \partial x_j}\right)(\ux_0)(x_i-x_{0,i})(x_j-x_{0,j})


which can be written as:

.. math::

    \widetilde{h}(\ux) = h(\ux_0) + \mat{L} (\ux-\ux_0) +  \frac{1}{2}  \left\langle \left\langle\mat{Q},\ux-
    \ux_0 \right \rangle, \ux-\ux_0 \right \rangle


where :math:`\mat{Q} = (Q_{ijk})_{1 \leq i,j \leq \inputDim, 1\leq k \leq \inputDim}` is the Hessian tensor of order 3 evaluated at :math:`\ux_0`:


.. math::

    Q_{ijk} = \frac{\partial^2 h_k}{\partial x_i \partial x_j} (\ux_0)

for :math:`i, j \in \{1, ..., \inputDim\}` and :math:`k \in \{1, ..., \inputDim\}`.

The first and second order Taylor expansions are used in the following cases:

- to evaluate the importance factors of the input point on the output: refer to :ref:`taylor_importance_factors`,

- to get an approximation of the mean and the variance of the output: refer to :ref:`taylor_expansion_moments`.


.. topic:: API:

    - See :class:`~openturns.LinearTaylor`
    - See :class:`~openturns.QuadraticTaylor`

.. topic:: Examples:

    - See :doc:`/auto_meta_modeling/general_purpose_metamodels/plot_taylor_approximation`

