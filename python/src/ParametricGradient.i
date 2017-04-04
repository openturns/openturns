// SWIG file ParametricGradient.i

OTAutodoc(ParametricGradient)

%{
#include "openturns/ParametricGradient.hxx"
%}

%include openturns/ParametricGradient.hxx
namespace OT { %extend ParametricGradient { ParametricGradient(const ParametricGradient & other) { return new OT::ParametricGradient(other); } } }
