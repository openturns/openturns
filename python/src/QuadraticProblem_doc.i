%feature("docstring") OT::QuadraticProblem
R"RAW(Quadratic optimization problem.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This defines a convex quadratic problem as:

.. math::

    \min_{\vect{x} \in B} f(\vect{x}) = \frac{1}{2} \vect{x}^T \mat{Q} \vect{x} + \vect{c}^T \vect{x} + d \\
    \vect{lc_l} \leq \mat{A} \vect{x} \leq \vect{lc_u}

where :math:`B` are the problem bounds, :math:`\mat{Q}` is the symmetric
positive semi-definite Hessian, :math:`\vect{c}` is the linear cost vector,
:math:`d` is a constant term, :math:`\mat{A}` is the constraint matrix and
:math:`\vect{lc_l}, \vect{lc_u}` are the constraint bounds.

The constant :math:`d` does not change the optimal point; it is included so
that :meth:`~openturns.OptimizationResult.getOptimalValue` returns the value
of the original objective, for instance the residual sum of squares when the
problem is built with
:meth:`~openturns.experimental.QuadraticProblem.FromLeastSquares`.

Parameters
----------
hessian : :class:`~openturns.SymmetricMatrix`
    The Hessian matrix :math:`\mat{Q}`.
linearCost : sequence of float
    The linear cost vector :math:`\vect{c}`.
bounds : :class:`~openturns.Interval`
    Bound constraints :math:`B`. An empty interval means no bounds.
A : :class:`~openturns.Matrix`
    Linear inequality constraints matrix :math:`\mat{A}`.
cb : :class:`~openturns.Interval`
    Linear inequality constraints bounds :math:`\vect{lc_l}, \vect{lc_u}`.

See Also
--------
openturns.experimental.HiGHS, openturns.experimental.LinearProblem

Examples
--------
Define and solve a bounded quadratic problem:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> Q = ot.SymmetricMatrix([[2.0, 0.0], [0.0, 2.0]])
>>> c = [-2.0, -4.0]
>>> bounds = ot.Interval([0.0, 0.0], [0.5, 10.0])
>>> problem = otexp.QuadraticProblem(Q, c, bounds)
>>> algo = otexp.HiGHS(problem)
>>> algo.run() # doctest: +SKIP
>>> result = algo.getResult() # doctest: +SKIP
>>> x_star = result.getOptimalPoint() # doctest: +SKIP
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::setHessian
"Accessor to the Hessian matrix.

Parameters
----------
hessian : :class:`~openturns.SymmetricMatrix`
    The symmetric positive semi-definite Hessian matrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::getHessian
"Accessor to the Hessian matrix.

Returns
-------
hessian : :class:`~openturns.SymmetricMatrix`
    The symmetric positive semi-definite Hessian matrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::setLinearCost
"Accessor to the linear cost.

Parameters
----------
linearCost : sequence of float
    Linear cost vector."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::getLinearCost
"Accessor to the linear cost.

Returns
-------
linearCost : :class:`~openturns.Point`
    Linear cost vector."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::setConstant
"Accessor to the constant term of the objective.

Parameters
----------
constant : float
    Constant term."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::getConstant
"Accessor to the constant term of the objective.

Returns
-------
constant : float
    Constant term."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::setLinearConstraint
"Accessor to the linear constraint term.

Parameters
----------
A : :class:`~openturns.Matrix`
    Linear inequality constraint matrix.
cb : :class:`~openturns.Interval`
    Linear inequality constraint bounds."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::getLinearConstraintCoefficients
"Accessor to the linear constraint coefficients.

Returns
-------
A : :class:`~openturns.Matrix`
    Linear inequality constraint matrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::getLinearConstraintBounds
"Accessor to the linear constraint bounds.

Returns
-------
cb : :class:`~openturns.Interval`
    Linear inequality constraint bounds."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::FromLeastSquares
R"RAW(Build a least-squares problem.

This builds the quadratic problem:

.. math::

    \min_{\vect{x} \in B} \norm{\mat{A} \vect{x} - \vect{b}}^2

which is equivalent to:

.. math::

    \min_{\vect{x} \in B} \frac{1}{2} \vect{x}^T \mat{Q} \vect{x} + \vect{c}^T \vect{x} + \norm{\vect{b}}^2

with :math:`\mat{Q} = 2 \mat{A}^T \mat{A}` and
:math:`\vect{c} = -2 \mat{A}^T \vect{b}`.

Parameters
----------
A : :class:`~openturns.Matrix`
    The design matrix.
b : sequence of float
    The right hand side.
bounds : :class:`~openturns.Interval`
    Bound constraints :math:`B`. An empty interval means no bounds.

Returns
-------
problem : :class:`~openturns.experimental.QuadraticProblem`
    The equivalent quadratic problem.
)RAW"
