%feature("docstring") OT::HODLRMatrixFactory
"Factory for HODLR matrices.

Notes
-----
This class is experimental."

%feature("docstring") OT::HODLRMatrixFactory::build
"Build an empty HODLR matrix of the correct size.

The caller must then call :py:meth:`~openturns.HODLRMatrix.assembleReal` and
:py:meth:`~openturns.HODLRMatrix.factorize` on the result.

Parameters
----------
sample : :class:`~openturns.Sample`
    Vertices on which the matrix is defined.
outputDimension : int
    Dimension of the covariance output.
symmetric : bool
    Whether the matrix is symmetric.
parameters : :class:`~openturns.HODLRMatrixParameters`, optional
    HODLR matrix parameters.

Returns
-------
HODLRMatrix : :class:`~openturns.HODLRMatrix`
    Empty HODLR matrix of size outputDimension * sample.getSize()."
