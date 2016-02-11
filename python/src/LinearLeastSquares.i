// SWIG file LinearLeastSquares.i

%{
#include "openturns/LinearLeastSquares.hxx"
%}

%include LinearLeastSquares_doc.i

%include openturns/LinearLeastSquares.hxx
namespace OT { %extend LinearLeastSquares { LinearLeastSquares(const LinearLeastSquares & other) { return new OT::LinearLeastSquares(other); } } }
