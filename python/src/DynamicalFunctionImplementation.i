// SWIG file DynamicalFunctionImplementation.i

%{
#include "openturns/DynamicalFunctionImplementation.hxx"
%}

%include DynamicalFunctionImplementation_doc.i

%include openturns/DynamicalFunctionImplementation.hxx
namespace OT { %extend DynamicalFunctionImplementation { DynamicalFunctionImplementation(const DynamicalFunctionImplementation & other) { return new OT::DynamicalFunctionImplementation(other); } } }
