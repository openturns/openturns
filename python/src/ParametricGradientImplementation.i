// SWIG file ParametricGradientImplementation.i

OTAutodoc(ParametricGradientImplementation)

%{
#include "openturns/ParametricGradientImplementation.hxx"
%}

%include openturns/ParametricGradientImplementation.hxx
namespace OT { %extend ParametricGradientImplementation { ParametricGradientImplementation(const ParametricGradientImplementation & other) { return new OT::ParametricGradientImplementation(other); } } }
