// SWIG file LeastSquaresMethod.i

%{
#include "openturns/LeastSquaresMethod.hxx"
%}

%include LeastSquaresMethod_doc.i

OTTypedInterfaceObjectHelper(LeastSquaresMethod)

%include openturns/LeastSquaresMethod.hxx
namespace OT { %extend LeastSquaresMethod { LeastSquaresMethod(const LeastSquaresMethod & other) { return new OT::LeastSquaresMethod(other); } } }
