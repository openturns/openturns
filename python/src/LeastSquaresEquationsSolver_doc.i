%feature("docstring") OT::LeastSquaresEquationsSolver
"Solve systems of non-linear equations using a least squares method.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Parameters
----------
absError : positive float
    Absolute error between two successive iterates at the end point.
    Default value is defined in the `ResourceMap` via the `Solver-DefaultAbsoluteError`
    key is set to :math:`10^{-5}`.
relError : positive float
    Relative error between the two last successive iterates with
    regards to the last iterate.
    Default value is defined in the `ResourceMap` via the `Solver-DefaultRelativeError`
    key is set to :math:`10^{-5}`.
resError : positive float
    Absolute threshold on `f(x)` for accepting a given solution. 
    It is also used as residual error for the least square optimizer.
    value.
    Default value is defined in the `ResourceMap` via the `Solver-DefaultResidualError`
    and is set to :math:`0`.
maximumFunctionEvaluation : int
    The maximum number of evaluations of the function. 
    Default value is defined in the `ResourceMap` via the `Solver-DefaultMaximumFunctionEvaluation`
    and is set to :math:`100`.

See also
--------
openturns.Solver, openturns.OptimizationAlgorithm

Notes
-----
LeastSquaresEquationsSolver might fail and not obtain a result lower than the
specified threshold, in this case an error is thrown.

Examples
--------

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> inputs = ['x', 'y']
>>> formulas = ['y*x-sin(2*x)', '1 + cos(y) + x']
>>> analytical = ot.SymbolicFunction(inputs, formulas)
>>> algo = otexp.LeastSquaresEquationsSolver()
>>> algo.setResidualError(1e-8)
>>> starting_point = [2.0, 1.0]
>>> solution = algo.solve(analytical, starting_point)
"

// ---------------------------------------------------------------------

%feature("docstring") OT::LeastSquaresEquationsSolver::solve
R"RAW(Solve a system of non-linear equations.


Parameters
----------
function : :class:`~openturns.Function`
    The vectorial function of the system of equations :math:`function_i(x) = 0 \forall i \in {1,...,n}` to be solved. Remark that it is an unbounded resolution.
startingPoint : sequence of floats
    The starting point of the optimization algorithm.
bounds : :class:`~openturns.Interval`
    Bounds for searching the solution of the system of equations.

Returns
-------
result : :class:`~openturns.Point`
    The result of the root research.

Notes
-----
LeastSquaresEquationsSolver might fail and not obtain a result lower than the
specified threshold, in this case an error is thrown.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LeastSquaresEquationsSolver::getOptimizationAlgorithm
"Get the used algorithm for the optimization.

Returns
-------
algorithm : :class:`~openturns.OptimizationAlgorithm`
    The used optimization algorithm.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::LeastSquaresEquationsSolver::setOptimizationAlgorithm
"Set the used algorithm for the optimization.

Parameters
----------
algorithm : :class:`~openturns.OptimizationAlgorithm`
    The optimization algorithm to be used.
"
