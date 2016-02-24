// SWIG file PiecewiseHermiteEvaluationImplementation.i

OTAutodoc(PiecewiseHermiteEvaluationImplementation)

%{
#include "openturns/PiecewiseHermiteEvaluationImplementation.hxx"
%}

%include openturns/PiecewiseHermiteEvaluationImplementation.hxx

namespace OT { %extend PiecewiseHermiteEvaluationImplementation { PiecewiseHermiteEvaluationImplementation(const PiecewiseHermiteEvaluationImplementation & other) { return new OT::PiecewiseHermiteEvaluationImplementation(other); } } }
