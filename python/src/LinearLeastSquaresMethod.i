// SWIG file LinearLeastSquaresMethod.i

%{
#include "openturns/LinearLeastSquaresMethod.hxx"
%}

%include LinearLeastSquaresMethod_doc.i

OTTypedInterfaceObjectHelper(LinearLeastSquaresMethod)

%apply const OT::LinearLeastSquaresMethod & { OT::LinearLeastSquaresMethod & };

%include openturns/LinearLeastSquaresMethod.hxx

%pythoncode %{
# Deprecated alias kept for backward compatibility, see issue #3254
LeastSquaresMethod = LinearLeastSquaresMethod
%}
