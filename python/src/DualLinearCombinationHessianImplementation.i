// SWIG file DualLinearCombinationHessianImplementation.i

%{
#include "openturns/DualLinearCombinationHessianImplementation.hxx"
%}

%include DualLinearCombinationHessianImplementation_doc.i

%include openturns/DualLinearCombinationHessianImplementation.hxx
namespace OT { %extend DualLinearCombinationHessianImplementation { DualLinearCombinationHessianImplementation(const DualLinearCombinationHessianImplementation & other) { return new OT::DualLinearCombinationHessianImplementation(other); } } }
