// SWIG file EnumerateFunctionImplementation.i

%{
#include "EnumerateFunctionImplementation.hxx"
%}

%include EnumerateFunctionImplementation_doc.i

%include EnumerateFunctionImplementation.hxx
namespace OT { %extend EnumerateFunctionImplementation { EnumerateFunctionImplementation(const EnumerateFunctionImplementation & other) { return new OT::EnumerateFunctionImplementation(other); } } }
