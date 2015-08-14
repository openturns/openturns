// SWIG file NumericalMathFunctionImplementation.i

%{
#include "NumericalMathFunctionImplementation.hxx"
%}

%include NumericalMathFunctionImplementation_doc.i

%template(EvaluationImplementation) OT::Pointer<OT::NumericalMathEvaluationImplementation>;
%template(GradientImplementation)   OT::Pointer<OT::NumericalMathGradientImplementation>;
%template(HessianImplementation)    OT::Pointer<OT::NumericalMathHessianImplementation>;

%include NumericalMathFunctionImplementation.hxx
namespace OT { %extend NumericalMathFunctionImplementation { NumericalMathFunctionImplementation(const NumericalMathFunctionImplementation & other) { return new OT::NumericalMathFunctionImplementation(other); } } }
