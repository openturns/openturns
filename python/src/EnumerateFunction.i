// SWIG file EnumerateFunction.i

%{
#include "EnumerateFunction.hxx"
%}

%include EnumerateFunction_doc.i

OTTypedInterfaceObjectHelper(EnumerateFunction)

%include EnumerateFunction.hxx
namespace OT { %extend EnumerateFunction { EnumerateFunction(const EnumerateFunction & other) { return new OT::EnumerateFunction(other); } } }
