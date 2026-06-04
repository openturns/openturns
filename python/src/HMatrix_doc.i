%feature("docstring") OT::HMatrix
OT_HMatrix_doc
%feature("docstring") OT::HMatrix::getDiagonal
OT_HMatrix_getDiagonal_doc
%feature("docstring") OT::HMatrix::addIdentity
OT_HMatrix_addIdentity_doc
%feature("docstring") OT::HMatrix::dump
OT_HMatrix_dump_doc
%feature("docstring") OT::HMatrix::copy
"Copy matrix.

As factorization overwrites matrix contents, this method
is useful to get a copy of assembled matrix before it is
factorized.

Returns
-------
matrix : :class:`~openturns.HMatrix`
    Matrix copy."
%feature("docstring") OT::HMatrix::transpose
OT_HMatrix_transpose_doc
%feature("docstring") OT::HMatrix::norm
OT_HMatrix_norm_doc
%feature("docstring") OT::HMatrix::getNbRows
OT_HMatrix_getNbRows_doc
%feature("docstring") OT::HMatrix::getNbColumns
OT_HMatrix_getNbColumns_doc
%feature("docstring") OT::HMatrix::assemble
OT_HMatrix_assemble_doc
%feature("docstring") OT::HMatrix::assembleReal
OT_HMatrix_assembleReal_doc
%feature("docstring") OT::HMatrix::assembleTensor
OT_HMatrix_assembleTensor_doc
%feature("docstring") OT::HMatrix::factorize
OT_HMatrix_factorize_doc
%feature("docstring") OT::HMatrix::solve
OT_HMatrix_solve_doc
%feature("docstring") OT::HMatrix::solveLower
OT_HMatrix_solveLower_doc
%feature("docstring") OT::HMatrix::compressionRatio
OT_HMatrix_compressionRatio_doc
%feature("docstring") OT::HMatrix::fullrkRatio
OT_HMatrix_fullrkRatio_doc
%feature("docstring") OT::HMatrix::scale
OT_HMatrix_scale_doc
%feature("docstring") OT::HMatrix::gemv
OT_HMatrix_gemv_doc
%feature("docstring") OT::HMatrix::gemm
OT_HMatrix_gemm_doc
