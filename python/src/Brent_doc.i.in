%feature("docstring") OT::Brent
"Brent algorithm solver for 1D non linear equations.

Parameters
----------
absError : positive float, optional
    Absolute error: distance between two successive iterates at the end point.
    Default is :math:`10^{-5}`.
relError : positive float, optional
    Relative error: distance between the two last successive iterates with
    regards to the last iterate. Default is :math:`10^{-5}`.
resError : positive float, optional
    Residual error: difference between the last iterate value and the expected
    value. Default is :math:`10^{-8}`.
maximumFunctionEvaluation : int, optional
    The maximum number of evaluations of the function. Default is :math:`100`.

See also
--------
Solver, Bisection, Secant

Notes
-----
The Brent solver is a mix of Bisection, Secant and inverse quadratic
interpolation.

If the function :math:`f` is continuous, the Brent solver will
converge towards a root of the equation :math:`function(x) = value` in
:math:`[infPoint, supPoint]`. If not, it will converge towards either a root or
a discontinuity point of :math:`f` on :math:`[infPoint, supPoint]`.
Bisection guarantees convergence.

Examples
--------

>>> import openturns as ot
>>> xMin = 0.0
>>> xMax= 3.0
>>> f = ot.MemoizeFunction(ot.SymbolicFunction('x', 'x^3-2*x^2-1'))
>>> solver = ot.Brent()
>>> root = solver.solve(f, 0.0, xMin, xMax)"

