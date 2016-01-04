// SWIG file ParametricGradientImplementation.i

OTAutodoc(ParametricGradientImplementation)

%{
#include "ParametricGradientImplementation.hxx"
%}

%include ParametricGradientImplementation.hxx
namespace OT { %extend ParametricGradientImplementation { ParametricGradientImplementation(const ParametricGradientImplementation & other) { return new OT::ParametricGradientImplementation(other); } } }
