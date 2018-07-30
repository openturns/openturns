.. _taylor_expansion:

Linear and Quadratic Taylor Expansions
--------------------------------------

| The approximation of the model response
  :math:`\underline{y} = h(\underline{x})` around a specific set
  :math:`\underline{x}_0 = (x_{0,1},\dots,x_{0,n_{X}})` of input
  parameters may be of interest. One may then substitute :math:`h` for
  its Taylor expansion at point :math:`\underline{x}_0`. Hence :math:`h`
  is replaced with a first or second-order polynomial
  :math:`\widehat{h}` whose evaluation is inexpensive, allowing the
  analyst to apply the uncertainty propagation methods.
| We consider the first and second order Taylor expansions around
  :math:`\ux=\underline{x}_0`.

  .. math::

    \underline{y} \, \, \approx \, \, \widehat{h}(\underline{x}) \, \, = \, \, h(\underline{x}_0) \, + \,  \sum_{i=1}^{n_{X}} \; \frac{\partial h}{\partial x_i}(\underline{x}_0).\left(x_i - x_{0,i} \right)

Introducing a vector notation, the previous equation rewrites:

.. math::

    \underline{y} \, \, \approx \, \,  \underline{y}_0 \, + \, \underline{\underline{L}} \: \left(\underline{x}-\underline{x}_0\right)

where:

-  :math:`\underline{y_0} = (y_{0,1} , \dots, y_{0,n_Y})^{\textsf{T}}=  h(\underline{x}_0)`
   is the vector model response evaluated at :math:`\underline{x}_0`;

-  :math:`\underline{x}` is the current set of input parameters;

-  :math:`\underline{\underline{L}} = \left( \frac{\partial y_{0,j}}{\partial x_i} \, \, , \, \, i=1,\ldots, n_X \, \, , \, \, j=1, \ldots, n_Y \right)`
   is the transposed Jacobian matrix evaluated at
   :math:`\underline{x}_0`.

  .. math::

     \begin{aligned}
         \underline{y} \, \, \approx \, \, \widehat{h}(\underline{x}) \, \, = \, \,
         h(\underline{x}_0) \, +  \, \sum_{i=1}^{n_{X}} \;  \frac{\partial h}{\partial x_i}(\underline{x}_0).\left(x_i - x_{0,i} \right) \, + \, \frac{1}{2} \; \sum_{i,j=1}^{n_X} \;  \frac{\partial^2 h}{\partial x_i \partial x_j}(\underline{x}_0).\left(x_i - x_{0,i} \right).\left(x_j - x_{0,j} \right)
       \end{aligned}

Introducing a vector notation, the previous equation rewrites:

.. math::

    \underline{y} \, \, \approx  \, \,  \underline{y}_0 \, + \,  \underline{\underline{L}} \: \left(\underline{x}-\underline{x}_0\right) \, + \,  \frac{1}{2} \; \left\langle \left\langle\underline{\underline{\underline{Q}}}\:,\underline{x}-\underline{x}_0 \right\rangle,\:\underline{x}-\underline{x}_0 \right\rangle

where
:math:`\underline{\underline{Q}} = \left\{ \frac{\partial^2 y_{0,k}}{\partial x_i \partial x_j} \, \, , \, \, i,j=1,\ldots, n_X \, \, , \, \, k=1, \ldots, n_Y \right\}`
is the transposed Hessian matrix.



.. topic:: API:

    - See :class:`~openturns.LinearTaylor`
    - See :class:`~openturns.QuadraticTaylor`

.. topic:: Examples:

    - See :doc:`/examples/meta_modeling/taylor_approximation`

