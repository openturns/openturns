%feature("docstring") OT::LinearTaylor
R"RAW(First order polynomial response surface by Taylor expansion.

Parameters
----------
center : sequence of float
    Point :math:`\ux_0`.
function : :class:`~openturns.Function`
    Function :math:`h` to be approximated at the point :math:`\ux_0`.

Notes
-----
The response surface is the first-order Taylor expansion of the function :math:`h` at the point :math:`\ux_0`. Refer to :ref:`Taylor Expansion <taylor_expansion>` for details.

See also
--------
QuadraticTaylor, LinearLeastSquares, QuadraticLeastSquares

Examples
--------
>>> import openturns as ot
>>> formulas = ['x1 * sin(x2)', 'cos(x1 + x2)', '(x2 + 1) * exp(x1 - 2 * x2)']
>>> myFunc = ot.SymbolicFunction(['x1', 'x2'], formulas)
>>> myTaylor = ot.LinearTaylor([1, 2], myFunc)
>>> myTaylor.run()
>>> responseSurface = myTaylor.getMetaModel()
>>> print(responseSurface([1.2,1.9]))
[1.13277,-1.0041,0.204127])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearTaylor::getCenter
R"RAW(Get the center.

Returns
-------
center : :class:`~openturns.Point`
    Point :math:`\ux_0` where the Taylor expansion of the function is
    performed.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearTaylor::getConstant
R"RAW(Get the constant vector of the approximation.

Returns
-------
constantVector : :class:`~openturns.Point`
    Point :math:`h(\ux_0)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearTaylor::getLinear
R"RAW(Get the gradient of the function at :math:`\ux_0`.

Returns
-------
gradient : :class:`~openturns.Matrix`
    The matrix :math:`\mat{L}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearTaylor::getInputFunction
"Get the function.

Returns
-------
function : :class:`~openturns.Function`
    Function :math:`h` to be approximated."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearTaylor::getMetaModel
R"RAW(Get the polynomial approximation of the function.

Returns
-------
approximation : :class:`~openturns.Function`
    The first-order Taylor expansiosn of :math:`h` at :math:`\ux_0`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearTaylor::run
R"RAW(Perform the first-order Taylor expansion around :math:`\ux_0`.)RAW"
