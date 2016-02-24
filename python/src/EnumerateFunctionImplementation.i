// SWIG file EnumerateFunctionImplementation.i

%{
#include "openturns/EnumerateFunctionImplementation.hxx"
%}

%include EnumerateFunctionImplementation_doc.i

%include openturns/EnumerateFunctionImplementation.hxx
namespace OT { %extend EnumerateFunctionImplementation { EnumerateFunctionImplementation(const EnumerateFunctionImplementation & other) { return new OT::EnumerateFunctionImplementation(other); } } }
