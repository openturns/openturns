%feature("docstring") OT::CMinpack
"Interface to CMinpack.

This class exposes a least-squares solver from the non-linear optimization library [cminpack2007]_.

Parameters
----------
problem : :class:`~openturns.OptimizationProblem`
    Least-squares optimization problem to solve.

Notes
-----
The algorithm relies on the lmder function, based on first-order derivative.
Box bound constraints are supported using a variable change.

More details are available `here <http://devernay.free.fr/hacks/cminpack/minpack-documentation.txt>`_.

See also
--------
AbdoRackwitz, Cobyla, NLopt, SQP, TNC

Examples
--------
>>> import openturns as ot
>>> dim = 2
>>> residualFunction = ot.SymbolicFunction(['x0', 'x1'], ['10*(x1-x0^2)', '0.5-x0', '0.3-2*x1'])
>>> problem = ot.LeastSquaresProblem(residualFunction)
>>> problem.setBounds(ot.Interval([-3.0] * dim, [5.0] * dim))
>>> algo = ot.CMinpack(problem)  # doctest: +SKIP
>>> algo.setStartingPoint([0.0] * dim)  # doctest: +SKIP
>>> algo.run()  # doctest: +SKIP
>>> result = algo.getResult()  # doctest: +SKIP
>>> x_star = result.getOptimalPoint()  # doctest: +SKIP
>>> y_star = result.getOptimalValue()  # doctest: +SKIP"

