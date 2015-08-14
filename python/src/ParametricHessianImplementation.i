// SWIG file ParametricHessianImplementation.i

%{
#include "ParametricHessianImplementation.hxx"
%}

%include ParametricHessianImplementation.hxx
namespace OT { %extend ParametricHessianImplementation { ParametricHessianImplementation(const ParametricHessianImplementation & other) { return new OT::ParametricHessianImplementation(other); } } }
