// SWIG file ParametricFunction.i

%{
#include "openturns/ParametricFunction.hxx"
%}

%include ParametricFunction_doc.i

%include openturns/ParametricFunction.hxx
namespace OT { %extend ParametricFunction { ParametricFunction(const ParametricFunction & other) { return new OT::ParametricFunction(other); } } }
