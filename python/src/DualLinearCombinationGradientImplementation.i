// SWIG file DualLinearCombinationGradientImplementation.i

%{
#include "DualLinearCombinationGradientImplementation.hxx"
%}

%include DualLinearCombinationGradientImplementation_doc.i

%include DualLinearCombinationGradientImplementation.hxx
namespace OT { %extend DualLinearCombinationGradientImplementation { DualLinearCombinationGradientImplementation(const DualLinearCombinationGradientImplementation & other) { return new OT::DualLinearCombinationGradientImplementation(other); } } }
