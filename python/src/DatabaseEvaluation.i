// SWIG file DatabaseEvaluation.i

%{
#include "openturns/DatabaseEvaluation.hxx"
%}

%include DatabaseEvaluation_doc.i

%include openturns/DatabaseEvaluation.hxx
namespace OT { %extend DatabaseEvaluation { DatabaseEvaluation(const DatabaseEvaluation & other) { return new OT::DatabaseEvaluation(other); } } }
