// SWIG file ParametricHessianImplementation.i

OTAutodoc(ParametricHessianImplementation)

%{
#include "openturns/ParametricHessianImplementation.hxx"
%}

%include openturns/ParametricHessianImplementation.hxx
namespace OT { %extend ParametricHessianImplementation { ParametricHessianImplementation(const ParametricHessianImplementation & other) { return new OT::ParametricHessianImplementation(other); } } }
