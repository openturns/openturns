// SWIG file LeastSquaresStrategy.i

%{
#include "openturns/LeastSquaresStrategy.hxx"
%}

%include LeastSquaresStrategy_doc.i

%include openturns/LeastSquaresStrategy.hxx
namespace OT{ %extend LeastSquaresStrategy { LeastSquaresStrategy(const LeastSquaresStrategy & other) { return new OT::LeastSquaresStrategy(other); } } }
