%feature("docstring") OT::QuadraticProblem
R"RAW(Quadratic optimization problem.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This defines a convex quadratic problem as:

.. math::

    \min_{\vect{x} \in B} f(\vect{x}) = \frac{1}{2} \vect{x}^T \mat{Q} \vect{x} + \vect{c}^T \vect{x} + d
    \quad \text{subject to} \quad \vect{L} \leq \mat{A} \vect{x} \leq \vect{U}

where :math:`B = \{\vect{x} \in \Rset^n : \vect{\ell} \leq \vect{x} \leq \vect{u}\}`
is the box defined by the problem bounds, :math:`\mat{Q}` is the symmetric
positive semi-definite Hessian, :math:`\vect{c}` is the linear cost vector and
:math:`d` is a constant term.

The rows of :math:`\mat{A}` define the linear constraints: row :math:`i` reads
:math:`L_i \leq \vect{A}_i \vect{x} \leq U_i`, where :math:`L_i` and :math:`U_i`
are the lower and upper bounds of the constraint interval. A one-sided
constraint is obtained with an infinite bound. An equality constraint is
obtained with equal lower and upper bounds, :math:`L_i = U_i`. The lower and
upper bounds :math:`\vect{\ell}` and :math:`\vect{u}` of :math:`B` are stored
as an :class:`~openturns.Interval`; an empty interval means that the problem is
unbounded.

The constant :math:`d` does not change the optimal point; it is included so
that :meth:`~openturns.OptimizationResult.getOptimalValue` returns the value
of the original objective, for instance the residual sum of squares when the
problem is built with
:meth:`~openturns.experimental.QuadraticProblem.FromLeastSquares`.

Available constructors:
    QuadraticProblem(*hessian, linearCost, bounds*)

    QuadraticProblem(*hessian, linearCost, bounds, constant*)

    QuadraticProblem(*hessian, linearCost, bounds, A, cb*)

    QuadraticProblem(*hessian, linearCost, bounds, A, cb, constant*)

Parameters
----------
hessian : :class:`~openturns.SymmetricMatrix`
    The Hessian matrix :math:`\mat{Q}`.
linearCost : sequence of float
    The linear cost vector :math:`\vect{c}`.
bounds : :class:`~openturns.Interval`
    Bound constraints :math:`B = \{\vect{x} : \vect{\ell} \leq \vect{x} \leq \vect{u}\}`.
    An empty interval means no bounds.
constant : float
    The constant term :math:`d`. Default is 0.
A : :class:`~openturns.Matrix`
    Linear constraint matrix :math:`\mat{A}`.
cb : :class:`~openturns.Interval`
    Linear constraint bounds :math:`\vect{L}, \vect{U}`.

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

Build the equivalent least-squares problem
:math:`\min_{\vect{x} \in B} \|\mat{A} \vect{x} - \vect{b}\|^2`:

>>> A = ot.Matrix([[1.0, 0.0], [0.0, 1.0], [1.0, 1.0]])
>>> b = [1.0, 2.0, 3.0]
>>> problem = otexp.QuadraticProblem.FromLeastSquares(A, b, bounds)
>>> problem.getConstant()
14.0
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

%feature("docstring") OT::QuadraticProblem::getEqualityConstraint
"Accessor to the equality constraints.

The equality constraints are derived from the linear constraints: a row whose
lower and upper bounds are equal, :math:`L_i = U_i`, defines the equality
constraint :math:`\\vect{A}_i \\vect{x} = L_i`.

Returns
-------
equality : :class:`~openturns.Function`
    Linear function :math:`\\vect{x} \\mapsto \\mat{A}_{eq} \\vect{x} - \\vect{L}_{eq}`
    built from the equality rows. Its input dimension is the problem
    dimension and its output dimension is the number of equality rows. When
    there is no equality row, the returned linear function has zero output
    dimension."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::setEqualityConstraint
"Set the equality constraint function.

QuadraticProblem only supports linear constraints: this method always raises
an exception. Define equality constraints through
:meth:`~openturns.experimental.QuadraticProblem.setLinearConstraint` with equal
lower and upper bounds.

Parameters
----------
equalityConstraint : :class:`~openturns.Function`
    Equality constraint function."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::hasEqualityConstraint
"Test whether equality constraints are defined.

Returns
-------
hasEqualityConstraint : bool
    *True* if at least one row of the linear constraints has equal lower and
    upper bounds."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::getInequalityConstraint
"Accessor to the inequality constraints.

The inequality constraints are derived from the linear constraints: each
finite bound of a row that is not an equality constraint gives one component,
with the convention :math:`h(\\vect{x}) \\geq 0`. The row
:math:`L_i \\leq \\vect{A}_i \\vect{x} \\leq U_i` gives the components
:math:`\\vect{A}_i \\vect{x} - L_i` and :math:`U_i - \\vect{A}_i \\vect{x}`.

Returns
-------
inequality : :class:`~openturns.Function`
    Linear function :math:`\\vect{x} \\mapsto \\mat{A}_{ineq} \\vect{x} - \\vect{L}_{ineq}`
    built from the inequality rows. Its input dimension is the problem
    dimension and its output dimension is the number of finite bounds of the
    rows that are not equality constraints. When there is no such row, the
    returned linear function has zero output dimension."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::setInequalityConstraint
"Set the inequality constraint function.

QuadraticProblem only supports linear constraints: this method always raises
an exception. Define inequality constraints through
:meth:`~openturns.experimental.QuadraticProblem.setLinearConstraint`.

Parameters
----------
inequalityConstraint : :class:`~openturns.Function`
    Inequality constraint function."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::hasInequalityConstraint
"Test whether inequality constraints are defined.

Returns
-------
hasInequalityConstraint : bool
    *True* if at least one row of the linear constraints has a finite bound
    and is not an equality constraint."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticProblem::setLinearConstraint
"Accessor to the linear constraints.

Each row :math:`i` of the constraint matrix defines
:math:`L_i \\leq \\vect{A}_i \\vect{x} \\leq U_i`, where :math:`L_i` and
:math:`U_i` are the bounds of the constraint interval. Use an infinite bound
for a one-sided constraint and equal lower and upper bounds for an equality
constraint.

Parameters
----------
A : :class:`~openturns.Matrix`
    Linear constraint matrix.
cb : :class:`~openturns.Interval`
    Linear constraint bounds."

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
