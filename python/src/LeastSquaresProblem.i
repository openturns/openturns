// SWIG file LeastSquaresProblem.i

%{
#include "openturns/LeastSquaresProblem.hxx"
%}

%include LeastSquaresProblem_doc.i

%include openturns/LeastSquaresProblem.hxx
namespace OT{ %extend LeastSquaresProblem { LeastSquaresProblem(const LeastSquaresProblem & other) { return new OT::LeastSquaresProblem(other); } } }
