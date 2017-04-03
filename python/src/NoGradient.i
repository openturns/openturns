// SWIG file NoGradient.i

OTAutodoc(NoGradient)

%{
#include "openturns/NoGradient.hxx"
%}

%include openturns/NoGradient.hxx
namespace OT { %extend NoGradient { NoGradient(const NoGradient & other) { return new OT::NoGradient(other); } } }
