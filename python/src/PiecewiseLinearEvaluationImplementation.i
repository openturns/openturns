// SWIG file PiecewiseLinearEvaluationImplementation.i

%{
#include "PiecewiseLinearEvaluationImplementation.hxx"
%}

%include PiecewiseLinearEvaluationImplementation.hxx

namespace OT { %extend PiecewiseLinearEvaluationImplementation { PiecewiseLinearEvaluationImplementation(const PiecewiseLinearEvaluationImplementation & other) { return new OT::PiecewiseLinearEvaluationImplementation(other); } } }
