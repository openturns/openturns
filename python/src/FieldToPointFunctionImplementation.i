// SWIG file FieldToPointFunctionImplementation.i

%{
#include "openturns/FieldToPointFunctionImplementation.hxx"
%}

%include FieldToPointFunctionImplementation_doc.i

%include openturns/FieldToPointFunctionImplementation.hxx
namespace OT { %extend FieldToPointFunctionImplementation { FieldToPointFunctionImplementation(const FieldToPointFunctionImplementation & other) { return new OT::FieldToPointFunctionImplementation(other); } } }
