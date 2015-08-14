// SWIG file LinearLeastSquares.i

%{
#include "LinearLeastSquares.hxx"
%}

%include LinearLeastSquares_doc.i

%include LinearLeastSquares.hxx
namespace OT { %extend LinearLeastSquares { LinearLeastSquares(const LinearLeastSquares & other) { return new OT::LinearLeastSquares(other); } } }
