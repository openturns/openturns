%define OT_LeastSquaresMethod_doc
R"RAW(Base class for least square solvers.

Available constructors:
    LeastSquaresMethod(*proxy, weight, indices*)

    LeastSquaresMethod(*proxy, indices*)

    LeastSquaresMethod(*design*)

Parameters
----------
proxy : :class:`~openturns.DesignProxy`
    Proxy of the design matrix: its element of row :math:`i` and column
    :math:`j` is :math:`\psi_j(\vect{u}_i)`, ie the value of the j-th basis
    term at the i-th point of the input sample.
weight : sequence of positive floats
    Statistical weights :math:`w_i` of the equations, of size equal to the
    size of the input sample.
    By default, all the weights are equal to 1.
indices : sequence of int
    Indices of the basis terms retained as columns of the design matrix.
    By default, all the terms are used.
design : 2-d sequence of float
    A priori known design matrix

See also
--------
CholeskyMethod, SVDMethod, QRMethod

Notes
-----
Solve the weighted least-squares problem:

.. math::

    \vect{a}  = \argmin_{\vect{b} \in \Rset^P}
    \left\|\mat{W}^{1/2} \left(\vect{y} - \mat{\Psi}(\mat{U}) \vect{b}\right)\right\|_2^2

where:

- :math:`\vect{y} \in \Rset^n` is the output sample,
- :math:`\mat{U}` is the input sample of :math:`n` points
  :math:`\vect{u}_i`,
- :math:`\mat{\Psi}(\mat{U})` is the design matrix: its element of row
  :math:`i` and column :math:`j` is :math:`\psi_j(\vect{u}_i)`,
- :math:`\mat{W} = \mathrm{diag}(w_1, \dots, w_n)` is the diagonal matrix
  built from the *weights*,
- the *indices* restrict the columns of :math:`\mat{\Psi}(\mat{U})` to the
  selected basis terms.

Examples
--------
>>> import openturns as ot
>>> A = ot.Matrix([[1, 1], [1, 2], [1, 3], [1, 4]])
>>> y = [6, 5, 7, 10]
>>> method = ot.LeastSquaresMethod(A)
>>> x = method.solve(y)
>>> print(x)
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

The returned vector minimizes the weighted norm of the residual with
respect to the second member :math:`\vect{b}`, as described in the
mathematical definition of the class:

.. math::

    \vect{a} = \argmin_{\vect{x} \in \Rset^P}
    \left\|\mat{W}^{1/2} \left(\mat{\Psi} \vect{x} - \vect{b}\right)\right\|_2^2

Parameters
----------
b : sequence of float
    Second member of the equation, typically the output sample.

Returns
-------
a : :class:`~openturns.Point`
    The solution.)RAW"
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::solve
OT_LeastSquaresMethod_solve_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_solveNormal_doc
R"RAW( Solve the least-squares problem using normal equation.

.. math::

    \mat{\Psi}^\intercal \mat{W} \, \mat{\Psi} \, \vect{x}
    = \mat{\Psi}^\intercal \mat{W} \, \vect{b}

Parameters
----------
b : sequence of float
    Second member of the equation.

Returns
-------
x : :class:`~openturns.Point`
    The solution.)RAW"
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::solveNormal
OT_LeastSquaresMethod_solveNormal_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getGramInverse_doc
R"RAW(Get the inverse Gram matrix of the input sample.

The weighted Gram matrix is defined as:

.. math::

    \mat{G} = \mat{\Psi}^\intercal \mat{W} \, \mat{\Psi}

Returns
-------
gramInverse : :class:`~openturns.CovarianceMatrix`
    The inverse :math:`\mat{G}^{-1}` of the weighted Gram matrix.)RAW"
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getGramInverse
OT_LeastSquaresMethod_getGramInverse_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getGramInverseDiag_doc
R"RAW(Get the diagonal of the inverse Gram matrix.

.. math::

    \mathrm{diag}(\mat{G}^{-1}) = \mathrm{diag}\left((\mat{\Psi}^\intercal \mat{W} \, \mat{\Psi})^{-1}\right)

Returns
-------
diagGramInverse : :class:`~openturns.Point`
    The diagonal of the inverse of the weighted Gram matrix.)RAW"
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getGramInverseDiag
OT_LeastSquaresMethod_getGramInverseDiag_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getGramInverseTrace_doc
R"RAW(Get the trace of the inverse Gram matrix.

.. math::

    \mathrm{Tr}(\mat{G}^{-1}) = \mathrm{Tr}\left((\mat{\Psi}^\intercal \mat{W} \, \mat{\Psi})^{-1}\right)

Returns
-------
trace : float
    The trace of the inverse of the weighted Gram matrix.)RAW"
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getGramInverseTrace
OT_LeastSquaresMethod_getGramInverseTrace_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getH_doc
R"RAW(Get the symmetric WLS kernel H.

.. math::

    \mat{H} = \mat{\Psi} \mat{G}^{-1} \mat{\Psi}^\intercal

Returns
-------
h : :class:`~openturns.SymmetricMatrix`
    The symmetric WLS kernel H. For non-unit weights, the fitted values
    are :math:`\mat{H}\mat{W}\vect{b}`.)RAW"
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getH
OT_LeastSquaresMethod_getH_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_getHDiag_doc
R"RAW(Get the diagonal of the symmetric WLS kernel H.

.. math::

    \mathrm{diag}(\mat{H}) = \mathrm{diag}\left(\mat{\Psi} \mat{G}^{-1} \mat{\Psi}^\intercal\right)

Returns
-------
diagH : :class:`~openturns.Point`
    The diagonal of the symmetric WLS kernel H. For non-unit weights, the
    fitted values are :math:`\mat{H}\mat{W}\vect{b}`.)RAW"
%enddef
%feature("docstring") OT::LeastSquaresMethodImplementation::getHDiag
OT_LeastSquaresMethod_getHDiag_doc

// ---------------------------------------------------------------------

%define OT_LeastSquaresMethod_computeWeightedDesign_doc
R"RAW(Build the design matrix.

The element of row :math:`i` and column :math:`j` of the returned matrix
is :math:`w_i^{1/2} \psi_j(\vect{u}_i)`: the rows are scaled by the square
root of the weights, and the columns are restricted to the selected
indices.

Parameters
----------
whole : bool, defaults to False
    Whether to use the initial indices instead of the current indices

Returns
-------
psiAk : :class:`~openturns.Matrix`
    The design matrix)RAW"
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


