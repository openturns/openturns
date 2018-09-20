.. _optimization_algorithm:

Optimization Algorithms
-----------------------

The method is used in the following context:
:math:`\vect{x}= \left( x^1,\ldots,x^{n_X} \right)` is a vector of
unknown variables, :math:`\vect{d}` a vector considered to be well known
or where uncertainty is negligible, and
:math:`y = h(\vect{x},\vect{d})` is the scalar variable of interest.
The objective here is to determine the extreme (minimum and maximum)
values of :math:`y` when :math:`\vect{x}` varies.

| It is possible to use some optimization algorithms. We give the
  principle of the TNC (Truncated Newton Constrained) algorithm which
  minimizes a function with variables subject to bounds, using gradient
  information.
| Truncated-Newton methods are a family of methods suitable for solving
  large nonlinear optimization problems. At each iteration, the current
  estimate of the solution is updated by approximately solving the
  Newton equations using an iterative algorithm. This results in a
  doubly iterative method: an outer iteration for the nonlinear
  optimization problem and an inner iteration for the Newton equations.
  The inner iteration is typically stopped or *truncated* before the
  solution to the Newton equations is obtained.
| The TNC algorithm resolves:
  :math:`\min_{\vect{x} \in [\vect{a},\vect{b}] \in \overline{\Rset}^n} f(\vect{x})`
  and proceeds as follows under the proper regularity of the objective
  function :math:`f`:

  .. math::

     \begin{aligned}
         \left\{
         \begin{array}{l}
           \vect{\nabla f}(\vect{x}) =\vect{0}  \\
           \mat{\nabla_2} f(\vect{x}) \mbox{ is definite positive}
         \end{array}
         \right.
       \end{aligned}

The Taylor development of second order of :math:`f` around
:math:`\vect{x}_k` leads to the determination of the iterate
:math:`\vect{x}_{k+1}` such as:

.. math::
  :label: linearSystem

    \left\{
    \begin{array}{lcl}
      \vect{\Delta}_k & = & \vect{x}_{k+1} - \vect{x}_k  \\
      \mat{\nabla_2} f(\vect{x}_k)\vect{\Delta}_k & = & -\vect{\nabla f}(\vect{x}_k)
    \end{array}
    \right.

The equation :eq:`linearSystem` is truncated: the iterative research of
:math:`\vect{\Delta}_k` is stopped as soon as :math:`\vect{\Delta}_k`
verifies :

.. math::

    || \mat{\nabla_2} f(\vect{x}_k)\vect{\Delta}_k + \vect{\nabla f}(\vect{x}_k) || \leq \eta ||\vect{\nabla f}(\vect{x}_k) ||

At last, the iteration :math:`k+1` is defined by:

.. math::

    \vect{x}_{k+1} = \vect{x}_k + \alpha_k \vect{\Delta}_k

where :math:`\alpha_k` is the parameter *stepmx*.


.. topic:: API:

    - See :class:`~openturns.TNC`
    - See the available :ref:`optimization algorithms <optimization_algorithms>`.

.. topic:: Examples:

    - See :doc:`/examples/numerical_methods/optimization_constraints`
    - See :doc:`/examples/numerical_methods/optimization_nlopt`

.. topic:: References:

    - [Nash1999]_

