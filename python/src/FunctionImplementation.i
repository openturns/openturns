// SWIG file FunctionImplementation.i

%{
#include "openturns/FunctionImplementation.hxx"
%}

%include FunctionImplementation_doc.i

%template(_EvaluationImplementationPointer) OT::Pointer<OT::EvaluationImplementation>;
%template(_GradientImplementationPointer)   OT::Pointer<OT::GradientImplementation>;
%template(_HessianImplementationPointer)    OT::Pointer<OT::HessianImplementation>;

%ignore OT::FunctionImplementation::getUseDefaultGradientImplementation;
%ignore OT::FunctionImplementation::setUseDefaultGradientImplementation;
%ignore OT::FunctionImplementation::getUseDefaultHessianImplementation;
%ignore OT::FunctionImplementation::setUseDefaultHessianImplementation;

%ignore OT::FunctionImplementation::isParallel;

%include openturns/FunctionImplementation.hxx
namespace OT { %extend FunctionImplementation { FunctionImplementation(const FunctionImplementation & other) { return new OT::FunctionImplementation(other); } } }
