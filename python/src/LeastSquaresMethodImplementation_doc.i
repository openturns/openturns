%define OT_LeastSquaresMethod_doc
R"RAW(Base class for least square solvers.

Available constructors:
    LeastSquaresMethod(*proxy, weight, indices*)

    LeastSquaresMethod(*proxy, indices*)

    LeastSquaresMethod(*design*)

Parameters
----------
proxy : :class:`~openturns.DesignProxy`
    The design proxy that builds the design matrix
weight : sequence of float
    The output weights
indices : sequence of int
    The indices allowed in the basis
design : 2-d sequence of float
    The apriori known design matrix

See also
--------
CholeskyMethod, SVDMethod, QRMethod

Notes
-----
This class finds :math:`\widehat{\mat{A}}` that minimizes the objective function  :math:`\cJ`:

.. math::
    :label: def_A_lsmethod

    \widehat{\mat{A}} = \argmin \cJ(\mat{A})


where :math:`\cJ` is defined by:

.. math::
    :label: func_obj_def

    \cJ(\mat{A}) = \sum_{i=1}^\sampleSize \left \| \vect{y}^{(i)} - \Tr{\mat{A}} \vect{\Psi(\vect{x}^{(i)})}
    \right \|_{L^2}^2

with :math:`\vect{\Psi(\vect{x}^{(i)})}` the *design matrix*.

The default method to solve the least squares problem is the *SVD* one. To specify another method, use the *Build* method.

Refer to :any:`least_squares` to get all the notations.

Examples
--------
>>> import openturns as ot
>>> design_matrix = ot.Matrix([[1, 1], [1, 2], [1, 3], [1, 4]])
>>> y = [6, 5, 7, 10]
>>> method = ot.LeastSquaresMethod(design_matrix)
>>> A = method.solve(y)
>>> print(A)
[3.5,1.4]
)RAW"
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation
OT_LeastSquaresMethod_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getInputSample_doc
"Input sample accessor.

Returns
-------
inputSample : :class:`~openturns.Sample`
    Input sample."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getInputSample
OT_LeastSquaresMethod_getInputSample_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getWeight_doc
"Accessor to the weights.

Returns
-------
weight : :class:`~openturns.Point`
    Weights."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getWeight
OT_LeastSquaresMethod_getWeight_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getBasis_doc
"Accessor to the basis.

Returns
-------
basis : collection of :class:`~openturns.Function`
    Basis."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getBasis
OT_LeastSquaresMethod_getBasis_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getCurrentIndices_doc
"Current indices accessor.

Returns
-------
indices : :class:`~openturns.Indices`
    Indices of the current decomposition in the global basis."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getCurrentIndices
OT_LeastSquaresMethod_getCurrentIndices_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getInitialIndices_doc
"Initial indices accessor.

Returns
-------
indices : :class:`~openturns.Indices`
    Initial indices of the terms in the global basis."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getInitialIndices
OT_LeastSquaresMethod_getInitialIndices_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_solve_doc
R"RAW(Solve the least-squares problem.

.. math::

    \vect{a} = \argmin_{\vect{x} \in \Rset^P} ||M\vect{x}-\vect{b}||^2

Parameters
----------
b : sequence of float
    Second term of the equation

Returns
-------
a : :class:`~openturns.Point`
    The solution.)RAW"
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::solve
OT_LeastSquaresMethod_solve_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_solveNormal_doc
"Solve the least-squares problem using normal equation.

.. math::

    M^T*M*x=M^T*b

Parameters
----------
b : sequence of float
    Second term of the equation

Returns
-------
x : :class:`~openturns.Point`
    The solution."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::solveNormal
OT_LeastSquaresMethod_solveNormal_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getGramInverse_doc
"Get the inverse Gram matrix of input sample.

.. math::

    G^{-1} = (X^T * X)^{-1}

Returns
-------
c : :class:`~openturns.CovarianceMatrix`
    The inverse Gram matrix."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getGramInverse
OT_LeastSquaresMethod_getGramInverse_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getGramInverseDiag_doc
"Get the diagonal of the inverse Gram matrix.

.. math::

    diag(G^{-1}) = diag((X^T * X)^{-1})

Returns
-------
d : :class:`~openturns.Point`
    The diagonal of the inverse Gram matrix."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getGramInverseDiag
OT_LeastSquaresMethod_getGramInverseDiag_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getGramInverseTrace_doc
"Get the trace of the inverse Gram matrix.

.. math::

    Tr(G^{-1}) = Tr(x^T * x)^{-1}

Returns
-------
x : float
    The trace of inverse Gram matrix."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getGramInverseTrace
OT_LeastSquaresMethod_getGramInverseTrace_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getH_doc
"Get the projection matrix H.

.. math::

    H = X * (X^T * X)^{-1} * X^T

Returns
-------
h : :class:`~openturns.SymmetricMatrix`
    The projection matrix H."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getH
OT_LeastSquaresMethod_getH_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getHDiag_doc
"Get the diagonal of the projection matrix H.

.. math::

    H = X * (X^T * X)^{-1} * X^T

Returns
-------
d : :class:`~openturns.Point`
    The diagonal of H."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getHDiag
OT_LeastSquaresMethod_getHDiag_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_computeWeightedDesign_doc
"Build the design matrix.

Parameters
----------
whole : bool, defaults to False
    Whether to use the initial indices instead of the current indices

Returns
-------
psiAk : :class:`~openturns.Matrix`
    The design matrix"
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::computeWeightedDesign
OT_LeastSquaresMethod_computeWeightedDesign_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_trashDecomposition_doc
"Drop the current decomposition."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::trashDecomposition
OT_LeastSquaresMethod_trashDecomposition_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_update_doc
"Update the current decomposition.

Parameters
----------
addedIndices : sequence of int
    Indices of added basis terms.
conservedIndices : sequence of int
    Indices of conserved basis terms.
removedIndices : sequence of int
    Indices of removed basis terms."
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::update
OT_LeastSquaresMethod_update_doc


