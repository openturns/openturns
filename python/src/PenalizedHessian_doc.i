%feature("docstring") OT::PenalizedHessian
R"RAW(Hessian which returns zeros on failure.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Useful for optimization of functions which may throw,
e.g. when the hessian fails outside of its domain of definition.
User interruptions are rethrown, not penalized.
This is consistent with :class:`~openturns.experimental.PenalizedEvaluation`:
the penalized function is locally constant on failure, hence its hessian is zero.

Parameters
----------
hessian : :class:`~openturns.HessianImplementation`
    Delegate hessian

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> f = ot.SymbolicFunction(['x'], ['x^2'])
>>> h = otexp.PenalizedHessian(f.getHessian())
>>> print(h.hessian([4.0]))
sheet #0
[[ 2 ]])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PenalizedHessian::getHessian
"Accessor to the internal hessian.

Returns
-------
hessian : :class:`~openturns.HessianImplementation`
    The internal hessian."

// ---------------------------------------------------------------------

%feature("docstring") OT::PenalizedHessian::setHessian
"Accessor to the internal hessian.

Parameters
----------
hessian : :class:`~openturns.HessianImplementation`
    The internal hessian."
