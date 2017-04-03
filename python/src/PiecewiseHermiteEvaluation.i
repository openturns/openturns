// SWIG file PiecewiseHermiteEvaluation.i

OTAutodoc(PiecewiseHermiteEvaluation)

%{
#include "openturns/PiecewiseHermiteEvaluation.hxx"
%}

%include openturns/PiecewiseHermiteEvaluation.hxx

namespace OT { %extend PiecewiseHermiteEvaluation { PiecewiseHermiteEvaluation(const PiecewiseHermiteEvaluation & other) { return new OT::PiecewiseHermiteEvaluation(other); } } }
