// SWIG file ConstantGradient.i

OTAutodoc(ConstantGradient)

%{
#include "openturns/ConstantGradient.hxx"
%}

%include openturns/ConstantGradient.hxx
namespace OT { %extend ConstantGradient { ConstantGradient(const ConstantGradient & other) { return new OT::ConstantGradient(other); } } }
