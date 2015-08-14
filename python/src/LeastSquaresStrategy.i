// SWIG file LeastSquaresStrategy.i

%{
#include "LeastSquaresStrategy.hxx"
%}

%include LeastSquaresStrategy_doc.i

%include LeastSquaresStrategy.hxx
namespace OT{ %extend LeastSquaresStrategy { LeastSquaresStrategy(const LeastSquaresStrategy & other) { return new OT::LeastSquaresStrategy(other); } } }
