// SWIG file LinearEvaluation.i

%{
#include "openturns/LinearEvaluation.hxx"
%}

%include LinearEvaluation_doc.i

%include openturns/LinearEvaluation.hxx
namespace OT { %extend LinearEvaluation { LinearEvaluation(const LinearEvaluation & other) { return new OT::LinearEvaluation(other); } } }
