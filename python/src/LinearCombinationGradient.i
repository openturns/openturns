// SWIG file LinearCombinationGradient.i

OTAutodoc(LinearCombinationGradient)

%{
#include "openturns/LinearCombinationGradient.hxx"
%}

%include openturns/LinearCombinationGradient.hxx
namespace OT { %extend LinearCombinationGradient { LinearCombinationGradient(const LinearCombinationGradient & other) { return new OT::LinearCombinationGradient(other); } } }
