// SWIG file PiecewiseLinearEvaluationImplementation.i

OTAutodoc(PiecewiseLinearEvaluationImplementation)

%{
#include "openturns/PiecewiseLinearEvaluationImplementation.hxx"
%}

%include openturns/PiecewiseLinearEvaluationImplementation.hxx

namespace OT { %extend PiecewiseLinearEvaluationImplementation { PiecewiseLinearEvaluationImplementation(const PiecewiseLinearEvaluationImplementation & other) { return new OT::PiecewiseLinearEvaluationImplementation(other); } } }
