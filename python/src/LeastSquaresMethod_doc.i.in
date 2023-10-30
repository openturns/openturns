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
%feature("docstring") OT::LeastSquaresMethod::trashDecomposition
OT_LeastSquaresMethod_trashDecomposition_doc
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
    Input sample
weight : sequence of float, optional
    Output weights
indices : sequence of int
    Indices allowed in the basis
design : 2-d sequence of float
    A priori known design matrix

Returns
-------
method : :class:`~openturns.LeastSquaresMethod`
    The built method

Examples
--------
>>> import openturns as ot
>>> basisSize = 3
>>> sampleSize = 5
>>> X = ot.Sample.BuildFromPoint(range(1, 1 + sampleSize))
>>> phis = [ot.SymbolicFunction(['x'], ['x^' + str(j + 1)]) for j in range(basisSize)]
>>> basis = ot.Basis(phis)
>>> proxy = ot.DesignProxy(X, phis)
>>> indices = range(basisSize)
>>> designMatrix = ot.Matrix(proxy.computeDesign(indices))
>>> method = ot.LeastSquaresMethod.Build('SVD', designMatrix)
>>> normal = ot.Normal([1.0] * sampleSize, [0.1] * sampleSize)
>>> y = normal.getRealization()
>>> x = method.solve(y)"
