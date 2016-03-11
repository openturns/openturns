// SWIG file CanonicalTensorEvaluation.i

%{
#include "openturns/CanonicalTensorEvaluation.hxx"
%}

%include CanonicalTensorEvaluation_doc.i

%include openturns/CanonicalTensorEvaluation.hxx
namespace OT { %extend CanonicalTensorEvaluation { CanonicalTensorEvaluation(const CanonicalTensorEvaluation & other) { return new OT::CanonicalTensorEvaluation(other); } } }
