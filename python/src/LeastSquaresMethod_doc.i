%feature("docstring") OT::LeastSquaresMethod
OT_LeastSquaresMethod_doc
%feature("docstring") OT::LeastSquaresMethod::computeWeightedDesign
OT_LeastSquaresMethod_computeWeightedDesign_doc
%feature("docstring") OT::LeastSquaresMethod::getInputSample
OT_LeastSquaresMethod_getInputSample_doc
%feature("docstring") OT::LeastSquaresMethod::getWeight
OT_LeastSquaresMethod_getWeight_doc
%feature("docstring") OT::LeastSquaresMethod::getBasis
OT_LeastSquaresMethod_getBasis_doc
%feature("docstring") OT::LeastSquaresMethod::getCurrentIndices
OT_LeastSquaresMethod_getCurrentIndices_doc
%feature("docstring") OT::LeastSquaresMethod::getInitialIndices
OT_LeastSquaresMethod_getInitialIndices_doc
%feature("docstring") OT::LeastSquaresMethod::solve
OT_LeastSquaresMethod_solve_doc
%feature("docstring") OT::LeastSquaresMethod::solveNormal
OT_LeastSquaresMethod_solveNormal_doc
%feature("docstring") OT::LeastSquaresMethod::getGramInverse
OT_LeastSquaresMethod_getGramInverse_doc
%feature("docstring") OT::LeastSquaresMethod::getGramInverseDiag
OT_LeastSquaresMethod_getGramInverseDiag_doc
%feature("docstring") OT::LeastSquaresMethod::getGramInverseTrace
OT_LeastSquaresMethod_getGramInverseTrace_doc
%feature("docstring") OT::LeastSquaresMethod::getH
OT_LeastSquaresMethod_getH_doc
%feature("docstring") OT::LeastSquaresMethod::getHDiag
OT_LeastSquaresMethod_getHDiag_doc
%feature("docstring") OT::LeastSquaresMethod::update
OT_LeastSquaresMethod_update_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::LeastSquaresMethod::Build
"Instantiate a decomposition method from its name.

Parameters
----------
name : str
    The name of the least-squares method
    Values are 'QR', 'SVD', 'Cholesky'
proxy : :class:`~openturns.DesignProxy`
    The design proxy that builds the design matrix
weight : sequence of float
    The output weights
indices : sequence of int
    The indices allowed in the basis
design : 2-d sequence of float
    The apriori known design matrix

Returns
-------
method : :class:`~openturns.LeastSquaresMethod`
    The built method

Notes
-----
This method implements a method to solve the least squares problem specified in
:eq:`def_A_lsmethod` and :eq:`func_obj_def`, using the specified method.

Refer to :any:`least_squares` to get all the notations.

Examples
--------
>>> import openturns as ot
>>> basisSize = 3
>>> sampleSize = 5
>>> x = ot.Sample.BuildFromPoint(range(1, 1 + sampleSize))
>>> phis = [ot.SymbolicFunction(['x'], ['x^' + str(j + 1)]) for j in range(basisSize)]
>>> basis = ot.Basis(phis)
>>> proxy = ot.DesignProxy(x, phis)
>>> indices = range(basisSize)
>>> designMatrix = ot.Matrix(proxy.computeDesign(indices))
>>> method = ot.LeastSquaresMethod.Build('SVD', designMatrix)
>>> normal = ot.Normal([1.0] * sampleSize, [0.1] * sampleSize)
>>> y = normal.getRealization()
>>> A = method.solve(y)"
