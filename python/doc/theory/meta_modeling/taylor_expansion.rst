.. _taylor_expansion:

Linear and Quadratic Taylor Expansions
--------------------------------------

We denote by :math:`h: \Rset^d \rightarrow \Rset^q` the function to be approximated and :math:`\uy=h(\ux)`.

**Linear Taylor expansion**

The linear Taylor expansion of :math:`h` at the point :math:`\ux_0` is the function :math:`\hat{h}: \Rset^d \rightarrow \Rset^q` defined for each marginal function :math:`h_k` of :math:`h`  by:

.. math::

   \hat{h}_k(\ux) = h_k(\ux_0) + \sum_{i=1}^{d} \left(\frac{\partial h_k}{\partial x_i}\right)(\ux_0)\left(x_i - x_{0,i} \right)

which can be written as:


.. math::

    \hat{h}(\ux) = h(\ux_0) + \mat{L} (\ux-\ux_0)


where :math:`\mat{L} = (L_{ij})_{1 \leq i \leq q, 1\leq j \leq d}` is the Jacobian matrix evaluated at :math:`\ux_0`:

.. math::

    L_{ij} = \left(\frac{\partial h_i}{\partial x_i}\right)(\ux_0)



**Quadratic Taylor expansion**

The quadratic Taylor expansion of :math:`h` at the point :math:`\ux_0` is the function :math:`\hat{h}: \Rset^d \rightarrow \Rset^q` defined for each marginal function :math:`h_k` of :math:`h`  by:

.. math::

   \hat{h}_k(\ux) = h_k(\ux_0) + \sum_{i=1}^{d}  \left(\frac{\partial h_k}{\partial x_i}\right)(\ux_0)\left(x_i - x_{0,i} \right) +  \frac{1}{2} \sum_{i,j = 1}^d \left( \frac{\partial^2 h_k}{\partial x_i \partial
      x_j}\right)(\ux_0)(x_i-x_{0,i})(x_j-x_{0,j})


which can be written as:

.. math::

    \hat{h}(\ux) = h(\ux_0) + \mat{L} (\ux-\ux_0) +  \frac{1}{2}  \left\langle \left\langle\mat{Q}, \ux-\ux_0 \right \rangle, \ux-\ux_0 \right \rangle


where :math:`\mat{Q} = (Q_{ijk})_{1 \leq i,j \leq d, 1\leq k \leq q}` is the Hessian tensor of dimension 3 evaluated at :math:`\ux_0`:


.. math::

    Q_{ijk} = \left(\frac{\partial^2 h_k}{\partial x_i\partial x_j}\right)(\ux_0)



.. topic:: API:

    - See :class:`~openturns.LinearTaylor`
    - See :class:`~openturns.QuadraticTaylor`

.. topic:: Examples:

    - See :doc:`/auto_meta_modeling/general_purpose_metamodels/plot_taylor_approximation`

