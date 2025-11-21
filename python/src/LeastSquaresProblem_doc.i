%feature("docstring") OT::LeastSquaresProblem
R"RAW(Least-squares problem.

This represents a least-squares problem:

.. math::

    \min_{x} ||f(\vect{x})||^2_2

where *f* is the residual function.

Parameters
----------
residualFunction : :class:`~openturns.Function`
    Residual function.

Examples
--------
>>> import openturns as ot
>>> residualFunction = ot.SymbolicFunction(['x0', 'x1'], ['10 * (x1 - x0^2)', '0.5 - x0', '0.3 - 2 * x1'])
>>> problem = ot.LeastSquaresProblem(residualFunction)
)RAW"
