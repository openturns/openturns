%feature("docstring") OT::SVDMethod
"Least squares solver using SVD decomposition.

Available constructors:
    SVDMethod(*proxy, weight, indices*)

    SVDMethod(*proxy, indices*)

    SVDMethod(*design*)

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
LeastSquaresMethod, CholeskyMethod, QRMethod

Notes
-----
This method implements a method to solve the least squares problem specified in
:eq:`def_A_lsmethod` and :eq:`func_obj_def`, using the SVD method.

Refer to :any:`least_squares` to get all the notations.

Examples
--------
Solves a linear least squares problem with SVD method:

>>> import openturns as ot
>>> A = ot.Matrix([[1, 1], [1, 2], [1, 3], [1, 4]])
>>> y = [6.0, 5.0, 7.0, 10.0]
>>> method = ot.SVDMethod(A)
>>> x = method.solve(y)
>>> print(x)
[3.5,1.4]"


