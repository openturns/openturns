// SWIG file QuadraticEvaluation.i

OTAutodoc(QuadraticEvaluation)

%{
#include "openturns/QuadraticEvaluation.hxx"
%}

%include openturns/QuadraticEvaluation.hxx
namespace OT { %extend QuadraticEvaluation { QuadraticEvaluation(const QuadraticEvaluation & other) { return new OT::QuadraticEvaluation(other); } } }
