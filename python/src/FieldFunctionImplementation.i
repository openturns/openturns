// SWIG file FieldFunctionImplementation.i

%{
#include "openturns/FieldFunctionImplementation.hxx"
%}

%include FieldFunctionImplementation_doc.i

%include openturns/FieldFunctionImplementation.hxx
namespace OT { %extend FieldFunctionImplementation { FieldFunctionImplementation(const FieldFunctionImplementation & other) { return new OT::FieldFunctionImplementation(other); } } }
