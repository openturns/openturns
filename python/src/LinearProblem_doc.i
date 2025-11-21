%feature("docstring") OT::LinearProblem
R"RAW(Linear optimization problem.

This defines a linear problem as:

.. math::

    \min_{\vect{x} \in B} f(\vect{x}) = \vect{c}^T \vect{x} \\
    \vect{lc_l} \leq \mat{A} \vect{x} \leq \vect{lc_u}

where :math:`B` are the problem bounds, :math:`\vect{c}` is the cost vector,
:math:`\mat{A}` is the constraint matrix term,
and :math:`\vect{cb_l}, \vect{cb_u}` are the constraints bounds terms.

Parameters
----------
cost : :class:`~openturns.Function`
    The cost vector :math:`\vect{c}`.
bounds : :class:`~openturns.Interval`
    Bound constraints :math:`B`
A : :class:`~openturns.Matrix`
    Linear inequality constraints matrix :math:`\mat{A}`
cb : :class:`~openturns.Interval`
    Linear inequality constraints bounds terms :math:`\vect{cb_l}, \vect{cb_u}`

Examples
--------
Define a mixed integer linear optimization problem with objective:

.. math::

    f = 1.1 x_0 + x_1, x_0 \in \Zset, x_1 \in \Rset

with inequality constraints:

.. math::
    :nowrap:

    \begin{eqnarray*}
        & x_1 & \leq 7\\
        5 & \leq x_0 + 2x_1 & \leq 15\\
        6 & \leq 3x_0 + 2x_1 & \\
    \end{eqnarray*}

and bound constraints:

.. math::
    :nowrap:

    \begin{eqnarray*}
        0 & \leq x_0 & \leq 4\\
        1 & \leq x_1 &
    \end{eqnarray*}

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> cost = [1.1, 1.0]
>>> bounds = ot.Interval([0.0, 1.0], [4.0, 1e30])
>>> A = ot.Matrix([[0.0, 1.0], [1.0, 2.0], [3.0, 2.0]])
>>> lcb = ot.Interval([-1e9, 5.0, 6.0], [7.0, 15.0, 1e9])
>>> problem = otexp.LinearProblem(cost, bounds, A, lcb)
>>> problem.setVariablesType([ot.OptimizationProblemImplementation.INTEGER, ot.OptimizationProblemImplementation.CONTINUOUS])
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearProblem::setLinearCost
"Accessor to the linear cost.

Parameters
----------
cost : :class:`~openturns.Point`
    Linear cost term.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearProblem::setLinearConstraint
"Accessor to the linear constraint term.

Returns
-------
A : :class:`~openturns.Matrix`
    Linear inequality constraint term.
cb : :class:`~openturns.Interval`
    Linear inequality constraint bounds.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearProblem::Linearize
"Transform a general optimization problem to a linear problem.

Objective and inequality constraints coefficients are computed
from their gradients at the specified location.

Parameters
----------
problem : :class:`~openturns.OptimizationProblem`
    The problem to linearize
location : sequence of float
    The linearization location

Returns
-------
linearProblem : :class:`~openturns.experimental.LinearProblem`
   The linear problem
"
  
