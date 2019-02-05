// SWIG file PiecewiseLinearEvaluation.i

%{
#include "openturns/PiecewiseLinearEvaluation.hxx"
%}

%include PiecewiseLinearEvaluation_doc.i

%include openturns/PiecewiseLinearEvaluation.hxx

namespace OT { %extend PiecewiseLinearEvaluation { PiecewiseLinearEvaluation(const PiecewiseLinearEvaluation & other) { return new OT::PiecewiseLinearEvaluation(other); } } }
