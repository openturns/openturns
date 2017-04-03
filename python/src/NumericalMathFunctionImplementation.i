// SWIG file NumericalMathFunctionImplementation.i

%{
#include "openturns/NumericalMathFunctionImplementation.hxx"
%}

%include NumericalMathFunctionImplementation_doc.i

%template(EvaluationImplementation) OT::Pointer<OT::EvaluationImplementation>;
%template(GradientImplementation)   OT::Pointer<OT::GradientImplementation>;
%template(HessianImplementation)    OT::Pointer<OT::HessianImplementation>;

%ignore OT::NumericalMathFunctionImplementation::getUseDefaultGradientImplementation;
%ignore OT::NumericalMathFunctionImplementation::setUseDefaultGradientImplementation;
%ignore OT::NumericalMathFunctionImplementation::getUseDefaultHessianImplementation;
%ignore OT::NumericalMathFunctionImplementation::setUseDefaultHessianImplementation;

%include openturns/NumericalMathFunctionImplementation.hxx
namespace OT { %extend NumericalMathFunctionImplementation { NumericalMathFunctionImplementation(const NumericalMathFunctionImplementation & other) { return new OT::NumericalMathFunctionImplementation(other); } } }
