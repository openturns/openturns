// SWIG file LeastSquaresMethod.i

%{
#include "openturns/LeastSquaresMethod.hxx"
%}

%include LeastSquaresMethod_doc.i

OTTypedInterfaceObjectHelper(LeastSquaresMethod)

%apply const OT::LeastSquaresMethod & { OT::LeastSquaresMethod & };

%include openturns/LeastSquaresMethod.hxx
