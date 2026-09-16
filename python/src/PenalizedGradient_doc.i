%feature("docstring") OT::PenalizedGradient
R"RAW(Gradient which returns zeros on failure.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Useful for optimization of functions which may throw,
e.g. when the gradient fails outside of its domain of definition.
User interruptions are rethrown, not penalized.
This is consistent with :class:`~openturns.experimental.PenalizedEvaluation`:
the penalized function is locally constant on failure, hence its gradient is zero.

Parameters
----------
gradient : :class:`~openturns.GradientImplementation`
    Delegate gradient

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> f = ot.SymbolicFunction(['x'], ['sqrt(x)'])
>>> g = otexp.PenalizedGradient(f.getGradient())
>>> print(g.gradient([4.0]))
[[ 0.25 ]]
>>> print(g.gradient([-1.0]))
[[ 0 ]])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PenalizedGradient::getGradient
"Accessor to the internal gradient.

Returns
-------
gradient : :class:`~openturns.GradientImplementation`
    The internal gradient."

// ---------------------------------------------------------------------

%feature("docstring") OT::PenalizedGradient::setGradient
"Accessor to the internal gradient.

Parameters
----------
gradient : :class:`~openturns.GradientImplementation`
    The internal gradient."
