// SWIG file MarginalGradient.i

OTAutodoc(MarginalGradient)

%{
#include "openturns/MarginalGradient.hxx"
%}

%include openturns/MarginalGradient.hxx
namespace OT { %extend MarginalGradient { MarginalGradient(const MarginalGradient & other) { return new OT::MarginalGradient(other); } } }
