// SWIG file ParametricEvaluationImplementation.i

OTAutodoc(ParametricEvaluationImplementation)

%{
#include "openturns/ParametricEvaluationImplementation.hxx"
%}

%include openturns/ParametricEvaluationImplementation.hxx
namespace OT { %extend ParametricEvaluationImplementation { ParametricEvaluationImplementation(const ParametricEvaluationImplementation & other) { return new OT::ParametricEvaluationImplementation(other); } } }
