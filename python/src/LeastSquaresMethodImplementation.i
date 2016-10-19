// SWIG file LeastSquaresMethodImplementation.i

%{
#include "openturns/LeastSquaresMethodImplementation.hxx"
%}

%include LeastSquaresMethodImplementation_doc.i

%include openturns/LeastSquaresMethodImplementation.hxx
namespace OT { %extend LeastSquaresMethodImplementation { LeastSquaresMethodImplementation(const LeastSquaresMethodImplementation & other) { return new OT::LeastSquaresMethodImplementation(other); } } }
