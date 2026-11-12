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
A : :class:`~openturns.Matrix` or :class:`~openturns.SparseMatrix`
    Linear inequality constraints matrix :math:`\mat{A}`. The sparse form
    avoids storing the zero coefficients of a large sparse problem.
cb : :class:`~openturns.Interval`
    Linear inequality constraints bounds terms :math:`\vect{cb_l}, \vect{cb_u}`

Notes
-----
The constraint matrix is stored in compressed sparse column form (see
:class:`~openturns.SparseMatrix`). Passing a dense :class:`~openturns.Matrix`
converts it once and drops its zero coefficients; passing a
:class:`~openturns.SparseMatrix` stores it as is. A large sparse problem
should therefore be built from a sparse matrix, as a dense one would need
:math:`8 m n` bytes for :math:`m` constraints and :math:`n` variables.

:meth:`getLinearConstraintCoefficients` rebuilds the dense matrix, while
:meth:`getSparseLinearConstraintCoefficients` returns the sparse coefficients
without densifying them.

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
>>> bounds = ot.Interval([0.0, 1.0], [4.0, 1e30], [True, True], [True, False])
>>> A = ot.Matrix([[0.0, 1.0], [1.0, 2.0], [3.0, 2.0]])
>>> lcb = ot.Interval([-1e9, 5.0, 6.0], [7.0, 15.0, 1e9], [False, True, True], [True, True, False])
>>> problem = otexp.LinearProblem(cost, bounds, A, lcb)
>>> problem.setVariablesType([ot.OptimizationProblemImplementation.INTEGER, ot.OptimizationProblemImplementation.CONTINUOUS])

The same problem can be defined from the sparse constraint matrix, which only
stores the non-zero coefficients. The triplets are given by their row index,
their column index and their value; duplicate coordinates are summed:

>>> A = ot.SparseMatrix(3, 2, [0, 1, 1, 2, 2], [1, 0, 1, 0, 1], [1.0, 1.0, 2.0, 3.0, 2.0])
>>> print(A.asDenseMatrix())
[[ 0 1 ]
 [ 1 2 ]
 [ 3 2 ]]
>>> problem = otexp.LinearProblem(cost, bounds, A, lcb)
>>> print(problem.getSparseLinearConstraintCoefficients().getNbNonZeros())
5
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

Parameters
----------
A : :class:`~openturns.Matrix` or :class:`~openturns.SparseMatrix`
    Linear inequality constraint term.
cb : :class:`~openturns.Interval`
    Linear inequality constraint bounds.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearProblem::getSparseLinearConstraintCoefficients
"Accessor to the linear constraint term in sparse form.

The constraint matrix is returned in compressed sparse column form, without
being densified.

Returns
-------
A : :class:`~openturns.SparseMatrix`
    Linear inequality constraint term.
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
  
