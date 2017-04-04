// SWIG file PiecewiseLinearEvaluation.i

OTAutodoc(PiecewiseLinearEvaluation)

%{
#include "openturns/PiecewiseLinearEvaluation.hxx"
%}

%include openturns/PiecewiseLinearEvaluation.hxx

namespace OT { %extend PiecewiseLinearEvaluation { PiecewiseLinearEvaluation(const PiecewiseLinearEvaluation & other) { return new OT::PiecewiseLinearEvaluation(other); } } }
