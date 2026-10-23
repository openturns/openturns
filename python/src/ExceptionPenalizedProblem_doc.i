%feature("docstring") OT::ExceptionPenalizedProblem
R"RAW(Optimization problem with failure penalty.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Wraps an :class:`~openturns.OptimizationProblem` and replaces its objective
by a penalized function using :class:`~openturns.experimental.ExceptionPenalizedEvaluation`,
so optimization algorithms can proceed when the objective throws.
The penalized values account for the minimization/maximization flags:
a failed evaluation returns a large positive value for minimization
and a large negative value for maximization, per output for
multi-objective problems.
Equality and inequality constraints are penalized as well:
a failed constraint evaluation is reported as infeasible
(+penalizedValue for equality constraints, -penalizedValue
for inequality constraints).
The penalized objective and constraints use finite-difference
gradient/hessian evaluated on the penalized evaluation, which never throws.
Analytic derivatives of the wrapped functions are not propagated.

.. note::
    The penalized functions are flat (constant) where the wrapped
    evaluation fails, so finite-difference gradients vanish there.
    Gradient-based solvers (TNC, SLSQP, LBFGS, ...) may mistake this null
    gradient for stationarity and converge silently to the penalized value.
    Use derivative-free solvers (Cobyla, Pagmo, Dlib, ...) and check that
    the optimal value differs from the penalized value.

Parameters
----------
problem : :class:`~openturns.OptimizationProblem`
    Wrapped optimization problem
penalizedValue : float, optional
    Penalized magnitude, finite and strictly positive. It is used for all outputs, with the sign
    deduced from the minimization flags.
    Default is ``sqrt(SpecFunc.MaxScalar)`` so the value stays finite
    if it is squared by the solver.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> f = ot.PythonFunction(1, 1, lambda x: [x[0] ** 0.5] if x[0] >= 0 else (_ for _ in ()).throw(RuntimeError('negative')))
>>> problem = ot.OptimizationProblem(f)
>>> penalizedProblem = otexp.ExceptionPenalizedProblem(problem, 1.0e6)
>>> print(penalizedProblem.getObjective()([-1.0]))
[1e+06])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ExceptionPenalizedProblem::getProblem
"Accessor to the wrapped problem.

Returns
-------
problem : :class:`~openturns.OptimizationProblem`
    The wrapped (unpenalized) problem."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExceptionPenalizedProblem::setProblem
"Accessor to the wrapped problem.

Parameters
----------
problem : :class:`~openturns.OptimizationProblem`
    The wrapped (unpenalized) problem."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExceptionPenalizedProblem::getPenalizedValue
"Accessor to the penalized magnitude.

Returns
-------
penalizedValue : float
    Penalized magnitude (signs are deduced from minimization flags)."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExceptionPenalizedProblem::setPenalizedValue
"Accessor to the penalized magnitude.

Parameters
----------
penalizedValue : float
    Penalized magnitude (signs are deduced from minimization flags).
    Must be finite and strictly positive."
