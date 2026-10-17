%feature("docstring") OT::LeastSquaresMethod
OT_LeastSquaresMethod_doc
%feature("docstring") OT::LeastSquaresMethod::computeWeightedDesign
OT_LeastSquaresMethod_computeWeightedDesign_doc
%feature("docstring") OT::LeastSquaresMethod::computeDesign
OT_LeastSquaresMethod_computeDesign_doc
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
%feature("docstring") OT::LeastSquaresMethod::solveNormalGram
OT_LeastSquaresMethod_solveNormalGram_doc
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
R"RAW(Instantiate a decomposition method from its name.

Parameters
----------
name : str
    The name of the least-squares method
    Values are 'QR', 'SVD', 'Cholesky'
proxy : :class:`~openturns.DesignProxy`
    Proxy of the design matrix: its element of row :math:`i` and column
    :math:`j` is :math:`\psi_j(\vect{u}_i)`, ie the value of the j-th basis
    term at the i-th point of the input sample.
weight : sequence of positive floats
    Weights :math:`w_i` of the equations, of size equal to the
    size of the input sample.

    By default, all the weights are equal to 1, which is equivalent,
    from an algebraic point of view, of weights all equal to :math:`1/size`.
indices : sequence of int
    Indices allowed in the basis

    By default, all the terms are used.
design : 2-d sequence of float
    The known design matrix

Returns
-------
method : :class:`~openturns.LeastSquaresMethod`
    The built method

Examples
--------
We get the projection onto the functional basis :math:`(x, x^2, x^3)` of
a model from which we only have a data base of
size 50. The least squares problem is solved using the SVD method.

There is no model selection strategy. The projection has 3 components:
all the functions of the basis are used.

>>> import openturns as ot
>>> sampleSize = 50
>>> X = ot.Sample.BuildFromPoint(range(1, 1 + sampleSize))
>>> normal = ot.Normal([1.0] * sampleSize, [0.1] * sampleSize)
>>> y = normal.getRealization()

Here we define the functional basis and the design proxy (for the last constructor):

>>> basisSize = 3
>>> phis = [ot.SymbolicFunction(['x'], ['x^' + str(j + 1)]) for j in range(basisSize)]
>>> proxy = ot.DesignProxy(X, phis)

Here, we explicit the design matrix:

>>> full = range(basisSize)
>>> designMatrix = ot.Matrix(proxy.computeDesign(full))

Here, we define and solve the least-squares problem:

>>> method = ot.LeastSquaresMethod.Build('SVD', designMatrix)
>>> x = method.solve(y)
)RAW"
