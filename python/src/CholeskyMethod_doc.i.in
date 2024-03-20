%feature("docstring") OT::CholeskyMethod
"Least squares solver using Cholesky decomposition.

Available constructors:
    CholeskyMethod(*proxy, weight, indices*)

    CholeskyMethod(*proxy, indices*)

    CholeskyMethod(*design*)

Parameters
----------
proxy : :class:`~openturns.DesignProxy`
    Input sample
weight : sequence of float
    Output weights
indices : sequence of int
    Indices allowed in the basis
design : 2-d sequence of float
    A priori known design matrix

See also
--------
LeastSquaresMethod, SVDMethod, QRMethod

Examples
--------
Solves a linear least squares problem with Cholesky method:

>>> import openturns as ot
>>> A = ot.Matrix([[1, 1], [1, 2], [1, 3], [1, 4]])
>>> y = [6, 5, 7, 10]
>>> method = ot.CholeskyMethod(A)
>>> x = method.solve(y)
>>> print(x)
[3.5,1.4]"

