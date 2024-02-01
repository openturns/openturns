// SWIG file EnumerateFunction.i

%{
#include "openturns/EnumerateFunction.hxx"
%}

%include EnumerateFunction_doc.i

OTTypedInterfaceObjectHelper(EnumerateFunction)

%copyctor OT::EnumerateFunction;

%include openturns/EnumerateFunction.hxx
