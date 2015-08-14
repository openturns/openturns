// SWIG file DynamicalFunctionImplementation.i

%{
#include "DynamicalFunctionImplementation.hxx"
%}

%include DynamicalFunctionImplementation_doc.i

%include DynamicalFunctionImplementation.hxx
namespace OT { %extend DynamicalFunctionImplementation { DynamicalFunctionImplementation(const DynamicalFunctionImplementation & other) { return new OT::DynamicalFunctionImplementation(other); } } }
