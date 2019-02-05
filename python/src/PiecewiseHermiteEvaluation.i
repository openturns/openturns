// SWIG file PiecewiseHermiteEvaluation.i

%{
#include "openturns/PiecewiseHermiteEvaluation.hxx"
%}

%include PiecewiseHermiteEvaluation_doc.i

%include openturns/PiecewiseHermiteEvaluation.hxx

namespace OT { %extend PiecewiseHermiteEvaluation { PiecewiseHermiteEvaluation(const PiecewiseHermiteEvaluation & other) { return new OT::PiecewiseHermiteEvaluation(other); } } }
