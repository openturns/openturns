// SWIG file TranslationEvaluation.i

%{
#include "openturns/TranslationEvaluation.hxx"
%}

%include TranslationEvaluation_doc.i

%include openturns/TranslationEvaluation.hxx
namespace OT { %extend TranslationEvaluation { TranslationEvaluation(const TranslationEvaluation & other) { return new OT::TranslationEvaluation(other); } } }
