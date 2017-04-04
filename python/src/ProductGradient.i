// SWIG file ProductGradient.i

OTAutodoc(ProductGradient)

%{
#include "openturns/ProductGradient.hxx"
%}

%include openturns/ProductGradient.hxx
namespace OT { %extend ProductGradient { ProductGradient(const ProductGradient & other) { return new OT::ProductGradient(other); } } }
