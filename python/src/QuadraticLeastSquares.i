// SWIG file QuadraticLeastSquares.i

%{
#include "QuadraticLeastSquares.hxx"
%}

%include QuadraticLeastSquares_doc.i

%include QuadraticLeastSquares.hxx
namespace OT { %extend QuadraticLeastSquares { QuadraticLeastSquares(const QuadraticLeastSquares & other) { return new OT::QuadraticLeastSquares(other); } } }
