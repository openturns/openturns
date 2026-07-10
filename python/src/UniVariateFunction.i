// SWIG file UniVariateFunction.i

%{
#include "openturns/UniVariateFunction.hxx"
%}

%include UniVariateFunction_doc.i

OTTypedInterfaceObjectHelper(UniVariateFunction)

%copyctor OT::UniVariateFunction;

%include openturns/UniVariateFunction.hxx
