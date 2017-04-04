// SWIG file DualLinearCombinationGradient.i

%{
#include "openturns/DualLinearCombinationGradient.hxx"
%}

%include DualLinearCombinationGradient_doc.i

%include openturns/DualLinearCombinationGradient.hxx
namespace OT { %extend DualLinearCombinationGradient { DualLinearCombinationGradient(const DualLinearCombinationGradient & other) { return new OT::DualLinearCombinationGradient(other); } } }
