// SWIG file DualLinearCombinationHessianImplementation.i

%{
#include "DualLinearCombinationHessianImplementation.hxx"
%}

%include DualLinearCombinationHessianImplementation_doc.i

%include DualLinearCombinationHessianImplementation.hxx
namespace OT { %extend DualLinearCombinationHessianImplementation { DualLinearCombinationHessianImplementation(const DualLinearCombinationHessianImplementation & other) { return new OT::DualLinearCombinationHessianImplementation(other); } } }
