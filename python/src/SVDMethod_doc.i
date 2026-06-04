%feature("docstring") OT::SVDMethod
"Least squares solver using SVD decomposition.

Available constructors:
    SVDMethod(*proxy, weight, indices*)

    SVDMethod(*proxy, indices*)

    SVDMethod(*design*)

Parameters
----------
proxy : :class:`~openturns.DesignProxy`
    The design proxy that builds the design matrix
weight : sequence of float
    The output weights
indices : sequence of int
    The indices allowed in the basis
design : 2-d sequence of float
    The known design matrix

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
>>> design_matrix = ot.Matrix([[1, 1], [1, 2], [1, 3], [1, 4]])
>>> y = [6, 5, 7, 10]
>>> method = ot.SVDMethod(design_matrix)
>>> A = method.solve(y)
>>> print(A)
[3.5,1.4]"


