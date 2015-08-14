// SWIG file PiecewiseHermiteEvaluationImplementation.i

%{
#include "PiecewiseHermiteEvaluationImplementation.hxx"
%}

%include PiecewiseHermiteEvaluationImplementation.hxx

namespace OT { %extend PiecewiseHermiteEvaluationImplementation { PiecewiseHermiteEvaluationImplementation(const PiecewiseHermiteEvaluationImplementation & other) { return new OT::PiecewiseHermiteEvaluationImplementation(other); } } }
