// SWIG file LeastSquaresExpansion.i

%{
#include "openturns/LeastSquaresExpansion.hxx"
%}

%include LeastSquaresExpansion_doc.i

%include openturns/LeastSquaresExpansion.hxx
namespace OT { %extend LeastSquaresExpansion { LeastSquaresExpansion(const LeastSquaresExpansion & other) { return new OT::LeastSquaresExpansion(other); } } }
