// SWIG file ProductEvaluation.i

OTAutodoc(ProductEvaluation)

%{
#include "openturns/ProductEvaluation.hxx"
%}

%include openturns/ProductEvaluation.hxx
namespace OT { %extend ProductEvaluation { ProductEvaluation(const ProductEvaluation & other) { return new OT::ProductEvaluation(other); } } }
