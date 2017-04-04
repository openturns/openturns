// SWIG file ParametricEvaluation.i

OTAutodoc(ParametricEvaluation)

%{
#include "openturns/ParametricEvaluation.hxx"
%}

%include openturns/ParametricEvaluation.hxx
namespace OT { %extend ParametricEvaluation { ParametricEvaluation(const ParametricEvaluation & other) { return new OT::ParametricEvaluation(other); } } }
