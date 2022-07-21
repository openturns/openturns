// SWIG file FunctionImplementation.i

%{
#include "openturns/FunctionImplementation.hxx"
%}

%include FunctionImplementation_doc.i

%template(EvaluationImplementation) OT::Pointer<OT::EvaluationImplementation>;
%template(GradientImplementation)   OT::Pointer<OT::GradientImplementation>;
%template(HessianImplementation)    OT::Pointer<OT::HessianImplementation>;

%ignore OT::FunctionImplementation::getUseDefaultGradientImplementation;
%ignore OT::FunctionImplementation::setUseDefaultGradientImplementation;
%ignore OT::FunctionImplementation::getUseDefaultHessianImplementation;
%ignore OT::FunctionImplementation::setUseDefaultHessianImplementation;

%ignore OT::FunctionImplementation::isParallel;

%include openturns/FunctionImplementation.hxx
namespace OT { %extend FunctionImplementation { FunctionImplementation(const FunctionImplementation & other) { return new OT::FunctionImplementation(other); } } }
