%feature("docstring") OT::SteplemanWinarsky
R"RAW(Adaptive numerical differentiation step search.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Implements the adaptive procedure of Stepleman and Winarsky,
which selects a finite-difference step balancing
truncation and round-off errors by successive reductions
:math:`h_{i+1} = \alpha h_i` with :math:`0 < \alpha < 1`.
The search is performed independently for each output marginal:
the result holds one optimal step vector per output.
The optimal steps are designed for the first-order centered finite
difference gradient: the result provides the model with its gradient
replaced by :class:`~openturns.CenteredFiniteDifferenceGradient` using
these steps.

For a function :math:`f` with continuous third derivative, the centered
finite difference :math:`D(h) = (f(x + h) - f(x - h)) / (2 h)` satisfies
:math:`f'(x) = D(h) - h^2 f'''(\\xi) / 6` with :math:`h > 0`.
Its total error combines the round-off error of the function evaluations
and the truncation error of the Taylor expansion,
:math:`e(h) = \\epsilon_f / h + h^2 |f'''(x)| / 6`,
where :math:`\\epsilon_f > 0` is the absolute precision of :math:`f`.
The optimal step :math:`h^\\star = (3 \\epsilon_f / |f'''(x)|)^{1/3}`
minimizes this error when the third derivative is nonzero.
Since the third derivative is generally unknown, the algorithm
approximates :math:`h^\\star` using function values only.

Parameters
----------
function : :class:`~openturns.Function`
    Model to differentiate.
x : :class:`~openturns.Point`
    Reference point where the step is estimated.
initialStep : :class:`~openturns.Point`
    Initial step value.

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``SteplemanWinarsky-DefaultAlpha`` (``Scalar``, default: ``0.25``): default value of the :math:`\alpha` shrink factor.
- ``SteplemanWinarsky-DefaultMaximumIterationNumber`` (``UnsignedInteger``, default: ``100``): default maximum number of iterations.

If the successive differences still decrease when the maximum number of
iterations is reached, :meth:`run` raises an error: increase the maximum
number of iterations (see :meth:`setMaximumIterationNumber`) or use a smaller
shrink factor (see :meth:`setAlpha`). :meth:`run` also raises an error if
no finite quotient can be evaluated (e.g. overflowing function values).

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> import math
>>> def scaled_exp(X):
...     alpha = 1.0e6
...     return [math.exp(-X[0] / alpha)]
>>> f = ot.PythonFunction(1, 1, scaled_exp)
>>> x = [1.0e-2]
>>> step0 = [1.0e8]
>>> algo = otexp.SteplemanWinarsky(f, x, step0)
>>> algo.run()
>>> result = algo.getResult()
>>> fOptStep = result.getFunctionWithOptimalStep()
>>> step = result.getOptimalStep()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::run
"Run the adaptive search, populates the result."

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::getResult
R"RAW(Accessor to the result.

Returns
-------
result : :class:`~openturns.experimental.SteplemanWinarskyResult`
    Result class holding the optimal steps.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::getFunction
R"RAW(Accessor to the model to differentiate.

Returns
-------
function : :class:`~openturns.Function`
    Model to differentiate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::setFunction
R"RAW(Accessor to the model to differentiate.

Parameters
----------
function : :class:`~openturns.Function`
    Model to differentiate.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::getX
R"RAW(Accessor to the point where the step is estimated.

Returns
-------
x : :class:`~openturns.Point`
    Point where the step is estimated.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::setX
R"RAW(Accessor to the point where the step is estimated.

Parameters
----------
x : :class:`~openturns.Point`
    Point where the step is estimated.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::getInitialStep
R"RAW(Accessor to the initial step value.

Returns
-------
initialStep : :class:`~openturns.Point`
    Initial step value.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::setInitialStep
R"RAW(Accessor to the initial step value.

Parameters
----------
initialStep : :class:`~openturns.Point`
    Initial step value.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::getAlpha
R"RAW(Accessor to the step reduction factor.

Returns
-------
alpha : float
    Shrink factor :math:`h_{i+1} = \alpha h_i`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::setAlpha
R"RAW(Accessor to the step reduction factor.

Parameters
----------
alpha : float
    Shrink factor, expected in ]0, 1[.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::getMaximumIterationNumber
R"RAW(Accessor to the maximum number of iterations.

Returns
-------
maximumIterationNumber : int
    Maximum number of iterations.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarsky::setMaximumIterationNumber
R"RAW(Accessor to the maximum number of iterations.

Parameters
----------
maximumIterationNumber : int
    Maximum number of iterations, expected at least 1.)RAW"
