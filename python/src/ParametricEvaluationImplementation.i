// SWIG file ParametricEvaluationImplementation.i

%{
#include "ParametricEvaluationImplementation.hxx"
%}

%include ParametricEvaluationImplementation.hxx
namespace OT { %extend ParametricEvaluationImplementation { ParametricEvaluationImplementation(const ParametricEvaluationImplementation & other) { return new OT::ParametricEvaluationImplementation(other); } } }
