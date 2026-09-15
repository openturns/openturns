%feature("docstring") OT::ExceptionPenalizedEvaluation
R"RAW(Evaluation with failure penalty, returning a large value on failure.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Useful for optimization of functions which may throw,
e.g. when the evaluation fails outside of its domain of definition.
User interruptions are rethrown, not penalized.

.. note::
    The failure-penalty function is flat (constant) where the delegate
    evaluation fails. Gradient-based optimization algorithms
    (TNC, SLSQP, LBFGS, ...) may mistake this null gradient for
    stationarity and converge silently to the penalized value.
    Prefer derivative-free algorithms (Cobyla, Pagmo, Dlib, ...).

Parameters
----------
evaluation : :class:`~openturns.Evaluation`
    Delegate evaluation
penalizedValue : float or sequence of float, optional
    Value returned when the delegate evaluation throws.
    If a single value is given it is used for all outputs.
    Default is ``sqrt(SpecFunc.MaxScalar)`` so the value stays finite
    if it is squared by the solver.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> f = ot.SymbolicFunction(['x'], ['sqrt(x)'])
>>> g = otexp.ExceptionPenalizedEvaluation(f.getEvaluation(), 1e6)
>>> print(g([4.0]))
[2]
>>> print(g([-1.0]))
[1e+06])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ExceptionPenalizedEvaluation::getEvaluation
"Accessor to the internal evaluation.

Returns
-------
evaluation : :class:`~openturns.Evaluation`
    The internal evaluation."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExceptionPenalizedEvaluation::setEvaluation
"Accessor to the internal evaluation.

Parameters
----------
evaluation : :class:`~openturns.Evaluation`
    The internal evaluation."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExceptionPenalizedEvaluation::getPenalizedValue
"Accessor to the penalized value.

Returns
-------
penalizedValue : :class:`~openturns.Point`
    Value returned when the internal evaluation throws."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExceptionPenalizedEvaluation::setPenalizedValue
"Accessor to the penalized value.

Parameters
----------
penalizedValue : float or sequence of float
    Value returned when the internal evaluation throws."
