%feature("docstring") OT::QuadraticTaylor
R"RAW(Second-order Taylor expansion.

Parameters
----------
center : sequence of float
    Point :math:`\ux_0`.
function : :class:`~openturns.Function`
    Function :math:`h` to be approximated at the point :math:`\ux_0`.

Notes
-----
The response surface is the second-order Taylor expansion of the function :math:`h` at the point :math:`\ux_0`. Refer to :ref:`Taylor Expansion <taylor_expansion>` for details.

See also
--------
LinearTaylor, LinearLeastSquares, QuadraticLeastSquares

Examples
--------
>>> import openturns as ot
>>> formulas = ['x1 * sin(x2)', 'cos(x1 + x2)', '(x2 + 1) * exp(x1 - 2 * x2)']
>>> myFunc = ot.SymbolicFunction(['x1', 'x2'], formulas)
>>> myTaylor = ot.QuadraticTaylor([1, 2], myFunc)
>>> myTaylor.run()
>>> responseSurface = myTaylor.getMetaModel()
>>> print(responseSurface([1.2,1.9]))
[1.13655,-0.999155,0.214084])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticTaylor::getCenter
R"RAW(Get the center.

Returns
-------
center : :class:`~openturns.Point`
    Point :math:`\ux_0` where the Taylor expansion of the function is
    performed.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticTaylor::getConstant
R"RAW(Get the constant vector of the approximation.

Returns
-------
constantVector : :class:`~openturns.Point`
    Point :math:`h(\ux_0)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticTaylor::getLinear
R"RAW(Get the gradient of the function at :math:`\ux_0`.

Returns
-------
gradient : :class:`~openturns.Matrix`
    The matrix :math:`\mat{L}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticTaylor::getQuadratic
R"RAW(Get the hessian of the function at :math:`\ux_0`.

Returns
-------
tensor : :class:`~openturns.SymmetricTensor`
    The tensor :math:`\mat{Q}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticTaylor::getInputFunction
"Get the function.

Returns
-------
function : :class:`~openturns.Function`
    Function :math:`h` to be approximated."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticTaylor::getMetaModel
R"RAW(Get the polynomial approximation of the function.

Returns
-------
approximation : :class:`~openturns.Function`
    The second-order Taylor expansion of :math:`h` at :math:`\ux_0`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticTaylor::run
R"RAW(Perform the second-order Taylor expansion around :math:`\ux_0`.)RAW"
