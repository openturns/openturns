%feature("docstring") OT::DesignProxy
R"RAW(Design matrix cache.

Available constructors:
    DesignProxy(*x, func_coll*)
    
    DesignProxy(*x, basis*)

    DesignProxy(*matrix*)

Parameters
----------
x : :class:`~openturns.Sample`
    Input sample
func_coll : sequence of :class:`~openturns.Function`
    Basis
basis : sequence of :class:`~openturns.Basis`
    Basis
matrix : 2-d sequence of float
The design matrix

Notes
-----
Let :math:`(\phi_k)_{1 \leq k \leq m}` be a functional basis of size :math:`m`, where each function
:math:`\phi_k: \Rset^\inputDim \rightarrow \Rset` with :math:`\inputDim \geq 1`.
Let :math:`(\vect{x}^j)_{1 \leq j \leq \sampleSize}` be some
input vectors in :math:`Rset^\inputDim`.

The class builds the matrix :math:`\mat{D} = (d_{jk})_{jk} \in \Rset^{\sampleSize\times m}` defined by:

.. math::
   d_{jk} = \phi_k(\vect{x}^j) \quad 1 \leq k \leq m, \quad 1 \leq j \leq \sampleSize

The matrix columns are effectively built when the *computeDesign* method is activated. The values are stored in a cache.
Can be useful for an iterative
least squares problem resolution or in interaction with :class:`~openturns.LeastSquaresMethod`
to select the algorithm used for the resolution of linear least-squares problems.

It is posisble to directly specify the values :math:`(d_{jk})_{jk}` in the last constructor.

Examples
--------
>>> import openturns as ot
>>> basisSize = 3
>>> sampleSize = 5
>>> X = ot.Sample(sampleSize, 1)
>>> X = ot.Sample.BuildFromPoint(range(1, 1 + sampleSize))
>>> func_coll = []
>>> for j in range(basisSize):
...     func_coll.append(ot.SymbolicFunction(['x'], ['x^' + str(j + 1)]))
>>> basis = ot.Basis(func_coll)
>>> proxy = ot.DesignProxy(X, func_coll)
>>> proxy2 = ot.DesignProxy(X, basis)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DesignProxy::getInputSample
"Input sample accessor.

Returns
-------
inputSample : :class:`~openturns.Sample`
    Input sample."

// ---------------------------------------------------------------------

%feature("docstring") OT::DesignProxy::getBasis
"Accessor to the basis.

Parameters
----------
indices : sequence of int, optional
    Indices of the active functions in the basis

Returns
-------
activeBasis : Collection of :class:`~openturns.Function`
    Collection of functions of the basis associated to the given indices. By default, the whole basis."

// ---------------------------------------------------------------------

%feature("docstring") OT::DesignProxy::computeDesign
"Build the design matrix.

Parameters
----------
indices : sequence of int
    Indices of the current basis in the global basis

Returns
-------
psiAk : :class:`~openturns.Matrix`
    The design matrix"

// ---------------------------------------------------------------------

%feature("docstring") OT::DesignProxy::getRowFilter
"Row filter accessor.

Returns
-------
rowFilter : :class:`~openturns.Indices`
    Sub-indices in of the sample in the current indices"

// ---------------------------------------------------------------------

%feature("docstring") OT::DesignProxy::getSampleSize
"Sample size accessor.

Returns
-------
sampleSize : int
    Size of sample accounting for row filter"

// ---------------------------------------------------------------------

%feature("docstring") OT::DesignProxy::setRowFilter
"Row filter accessor.

Parameters
----------
rowFilter : sequence of int
    Sub-indices in of the sample in the current indices"

// ---------------------------------------------------------------------

%feature("docstring") OT::DesignProxy::hasRowFilter
"Row filter flag accessor.

Returns
-------
hasRowFilter : bool
    Whether sub-indices of the basis are set"
