// SWIG file LeastSquaresMethod.i

%{
#include "openturns/LeastSquaresMethod.hxx"
%}

%include LeastSquaresMethod_doc.i

OTTypedInterfaceObjectHelper(LeastSquaresMethod)

%copyctor OT::LeastSquaresMethod;

%include openturns/LeastSquaresMethod.hxx
