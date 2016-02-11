// SWIG file EnumerateFunction.i

%{
#include "openturns/EnumerateFunction.hxx"
%}

%include EnumerateFunction_doc.i

OTTypedInterfaceObjectHelper(EnumerateFunction)

%include openturns/EnumerateFunction.hxx
namespace OT { %extend EnumerateFunction { EnumerateFunction(const EnumerateFunction & other) { return new OT::EnumerateFunction(other); } } }
