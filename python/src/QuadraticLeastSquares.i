// SWIG file QuadraticLeastSquares.i

%{
#include "openturns/QuadraticLeastSquares.hxx"
%}

%include QuadraticLeastSquares_doc.i

%include openturns/QuadraticLeastSquares.hxx
namespace OT { %extend QuadraticLeastSquares { QuadraticLeastSquares(const QuadraticLeastSquares & other) { return new OT::QuadraticLeastSquares(other); } } }
