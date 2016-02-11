// SWIG file DualLinearCombinationGradientImplementation.i

%{
#include "openturns/DualLinearCombinationGradientImplementation.hxx"
%}

%include DualLinearCombinationGradientImplementation_doc.i

%include openturns/DualLinearCombinationGradientImplementation.hxx
namespace OT { %extend DualLinearCombinationGradientImplementation { DualLinearCombinationGradientImplementation(const DualLinearCombinationGradientImplementation & other) { return new OT::DualLinearCombinationGradientImplementation(other); } } }
