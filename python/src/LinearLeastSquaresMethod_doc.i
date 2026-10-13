%feature("docstring") OT::LinearLeastSquaresMethod
OT_LinearLeastSquaresMethod_doc
%feature("docstring") OT::LinearLeastSquaresMethod::computeWeightedDesign
OT_LinearLeastSquaresMethod_computeWeightedDesign_doc
%feature("docstring") OT::LinearLeastSquaresMethod::computeDesign
OT_LinearLeastSquaresMethod_computeDesign_doc
%feature("docstring") OT::LinearLeastSquaresMethod::getInputSample
OT_LinearLeastSquaresMethod_getInputSample_doc
%feature("docstring") OT::LinearLeastSquaresMethod::getWeight
OT_LinearLeastSquaresMethod_getWeight_doc
%feature("docstring") OT::LinearLeastSquaresMethod::getBasis
OT_LinearLeastSquaresMethod_getBasis_doc
%feature("docstring") OT::LinearLeastSquaresMethod::getCurrentIndices
OT_LinearLeastSquaresMethod_getCurrentIndices_doc
%feature("docstring") OT::LinearLeastSquaresMethod::getInitialIndices
OT_LinearLeastSquaresMethod_getInitialIndices_doc
%feature("docstring") OT::LinearLeastSquaresMethod::solve
OT_LinearLeastSquaresMethod_solve_doc
%feature("docstring") OT::LinearLeastSquaresMethod::solveNormal
OT_LinearLeastSquaresMethod_solveNormal_doc
%feature("docstring") OT::LinearLeastSquaresMethod::solveNormalGram
OT_LinearLeastSquaresMethod_solveNormalGram_doc
%feature("docstring") OT::LinearLeastSquaresMethod::getGramInverse
OT_LinearLeastSquaresMethod_getGramInverse_doc
%feature("docstring") OT::LinearLeastSquaresMethod::getGramInverseDiag
OT_LinearLeastSquaresMethod_getGramInverseDiag_doc
%feature("docstring") OT::LinearLeastSquaresMethod::getGramInverseTrace
OT_LinearLeastSquaresMethod_getGramInverseTrace_doc
%feature("docstring") OT::LinearLeastSquaresMethod::getH
OT_LinearLeastSquaresMethod_getH_doc
%feature("docstring") OT::LinearLeastSquaresMethod::getHDiag
OT_LinearLeastSquaresMethod_getHDiag_doc
%feature("docstring") OT::LinearLeastSquaresMethod::update
OT_LinearLeastSquaresMethod_update_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquaresMethod::Build
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
method : :class:`~openturns.LinearLeastSquaresMethod`
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
>>> method = ot.LinearLeastSquaresMethod.Build('SVD', designMatrix)
>>> normal = ot.Normal([1.0] * sampleSize, [0.1] * sampleSize)
>>> y = normal.getRealization()
>>> x = method.solve(y)"
