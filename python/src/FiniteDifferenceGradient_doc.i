%feature("docstring") OT::FiniteDifferenceGradient
"Base class for first order finite-difference schemes.

Available constructors:
    FiniteDifferenceGradient(*epsilon, evalImpl*)

    FiniteDifferenceGradient(*step, evalImpl*)

Parameters
----------
evalImpl : :class:`~openturns.EvaluationImplementation`
    Implementation of the evaluation of a function.
epsilon : float, sequence of float
    Finite difference steps for each dimension.
step : :class:`~openturns.FiniteDifferenceStep`
    Defines how finite difference steps values are computed.

Notes
-----
Base class to define first order finite-difference schemes. The gradient
can be computed only through its derived classes:

- :class:`~openturns.CenteredFiniteDifferenceGradient`,

- :class:`~openturns.NonCenteredFiniteDifferenceGradient`."

// ---------------------------------------------------------------------

%feature("docstring") OT::FiniteDifferenceGradient::getInputDimension
"Get the input dimension.

Returns
-------
dimension : int
    Input dimension."

// ---------------------------------------------------------------------

%feature("docstring") OT::FiniteDifferenceGradient::getOutputDimension
"Get the output dimension.

Returns
-------
dimension : int
    Output dimension."

// ---------------------------------------------------------------------

%feature("docstring") OT::FiniteDifferenceGradient::getEpsilon
"Get the finite difference steps.

Returns
-------
epsilon : :class:`~openturns.Point`
    Finite difference steps for each dimension."

// ---------------------------------------------------------------------

%feature("docstring") OT::FiniteDifferenceGradient::getEvaluation
"Get the implementation of the evaluation of the function.

Returns
-------
evalImpl : :class:`~openturns.EvaluationImplementation`
    Implementation of the evaluation of a function."

// ---------------------------------------------------------------------

%feature("docstring") OT::FiniteDifferenceGradient::gradient
"Get the gradient at some point.

Parameters
----------
point : sequence of float
    Point where the gradient is computed.

Returns
-------
gradient : :class:`~openturns.Matrix`
    Transposed Jacobian matrix evaluated at *point*."

// ---------------------------------------------------------------------

%feature("docstring") OT::FiniteDifferenceGradient::getFiniteDifferenceStep
"Get the finite difference step.

Returns
-------
step : :class:`~openturns.FiniteDifferenceStep`
    Defines how finite difference steps values are computed."

// ---------------------------------------------------------------------

%feature("docstring") OT::FiniteDifferenceGradient::setFiniteDifferenceStep
"Set the finite difference step.

Parameters
----------
step : :class:`~openturns.FiniteDifferenceStep`
    Defines how finite difference steps values are computed."
